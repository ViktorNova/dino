/*****************************************************************************
    libdinoseq - a library for MIDI sequencing
    Copyright (C) 2009  Lars Luthman <mail@larsluthman.net>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef NODESKIPLIST_HPP
#define NODESKIPLIST_HPP

#include <cstdlib>
#include <limits>
#include <memory>

#include "atomicptr.hpp"
#include "meta.hpp"


namespace Dino {
  
  
  /** A basic skip list. This is a very basic list, it has no
      iterator interface and the user is responsible for allocating and
      deallocating the Node objects before inserting and after removing
      them, using @c new and @c delete. The only exception is when the
      destructor for the list is called, at which point all nodes still
      in the list will be deallocated using @c delete.
      
      This list type is more suited to be used as a building block for more 
      complex data structures than as a stand-alone skip list. All operations
      except construction and destruction are thread-safe and lock-free
      as long as only one thread is calling insert() and remove().
      
      @tparam T the payload type of the skiplist
      @tparam K the inverse of the probability that a node should have links
                at level N, given that it has links at level N-1
      @tparam M the maximum number of levels
  */
  template <typename T, int K = 2, int M = 20>
  class NodeSkipList {
  public:
    
    
    struct NodeBase;
    
    
    /** The data structure that holds the actual links. Each NodeBase
	has an array of LinkNode objects that contain links to the previous
	and next nodes at different levels. Only the @c next element is
	atomic since that is the only one that may be read by multiple 
	threads.*/
    struct LinkNode {
      
      /** This constructor sets the prev and next pointers to 0. */
      LinkNode() : prev(0), next(0) {}
      
      /** A pointer to the previous node in the list at this level. */
      NodeBase* prev;
      
      /** A pointer to the next node in the list at this level. */
      AtomicPtr<NodeBase> next;
    };
    
    
    /** A base struct for Node. This only has the links and is used for the
	head and end markers so we don't have to use Node with its
	potentially expensive data member for that. */
    struct NodeBase {
      
      /** Constructs a new NodeBase. */
      NodeBase(size_t l) throw() 
	: levels(l), 
	  links(l > 0 ? new LinkNode[levels] : 0) {}
      
      /** The number of elements in links. */
      size_t levels;
      
      /** The links to the previous and next nodes on different levels. */
      std::unique_ptr<LinkNode[]> links;
    };
    
    
    /** The node type of NodeSkipList. It inherits NodeBase and adds the data
	member. */
    struct Node : NodeBase {
      
      /** Constructs a new node with the given data. This function will
	  not throw any exceptions unless the copy constructor for @c T
	  does. */
      Node(T const& d, size_t l = 0) 
	: NodeBase(l), 
	  data(d) {
	if (this->levels == 0) {
	  do {
	    ++(this->levels);
	  } while (this->levels <= M && (std::rand() % K == 0));
	}
	this->links = std::unique_ptr<LinkNode[]>(new LinkNode[this->levels]);
      }
      
      /** Constructs a new node with the given data, which may be moved.
	  This function will not throw any exceptions unless the move
	  constructor for @c T does. */
      Node(T&& d, size_t levels) : NodeBase(levels), data(std::move(d)) {}
      
      /** The data element of this list node. */
      T data;
    };
    
    
    /** Construct an empty list. */
    NodeSkipList() throw() 
      : m_head(M),
	m_end(M) {
      for (int l = 0; l < M; ++l) {
	m_head.links[l].next.set(&m_end);
	m_end.links[l].prev = &m_head;
      }
    }
    
    /** Release all memory used by the list and its nodes. */
    ~NodeSkipList() throw() {
      NodeBase* nb = m_head.links[0].next.get();
      while (nb != &m_end) {
	Node* n = static_cast<Node*>(nb);
	nb = nb->links[0].next.get();
	delete n;
      }
    }
    
    /** Returns the first node in the list. You can use it to insert nodes
	at the beginning using list.insert(list.first_node(), my_node).
	This returns a NodeBase instead of a Node since the list may be
	empty, in which case this function returns the same as end_marker().
	However, if the return value differs from that of end_marker() it is
	safe to use @c static_cast to cast it to a Node pointer.
	
	This function is atomic and a memory barrier. */
    NodeBase* first_node() throw() {
      return m_head.links[0].next.get();
    }
    
    /** Returns the first node in the list. You can use it to insert nodes
	at the beginning using list.insert(list.first_node(), my_node).
	This returns a NodeBase instead of a Node since the list may be
	empty, in which case this function returns the same as end_marker().
	However, if the return value differs from that of end_marker() it is
	safe to use @c static_cast to cast it to a Node pointer. 
    
	This function is atomic and a memory barrier. */
    NodeBase const* first_node() const throw() {
      return m_head.links[0].next.get();
    }
    
    /** Returns a pointer to the end marker of the list. You can compare
	it to the return value of find_less(). */
    NodeBase const* head_marker() const throw() {
      return &m_head;
    }
    
    /** Returns a pointer to the end marker of the list. You can use it
	to insert nodes at the end using 
	list.insert(list.end_marker(), my_node) or compare it to return
	values of find(). */
    NodeBase* end_marker() throw() {
      return &m_end;
    }
    
    /** Returns a const pointer to the end marker of the list. You can 
	compare it to return values of find(). */
    NodeBase const* end_marker() const throw() {
      return &m_end;
    }
    
    /** Insert a new node into the list at a given position. The node will
	be inserted before the NodeBase @c before, which must either be a
	pointer to a node already in the list, or end_marker(). The list 
	assumes ownership of the object pointed to by @c node and will 
	deallocate it in the list destructor using @c delete unless the 
	node has been removed from the list before that. This means that 
	the Node object @b must be allocated using @c new unless you are 
	completely sure that it will be removed before the list destructor 
	is called.
	
	If inserting the new Node at the given position would break the order
	of the list it will not be inserted and the function will return 
	@c false. Otherwise it returns true. */
    bool insert(NodeBase* before, Node* node) throw() {
      
      // Check that we can insert the node in this position.
      if ((before != end_marker() &&
	   static_cast<Node*>(before)->data < node->data) ||
	  (before->links[0].prev != head_marker() &&
	   node->data < static_cast<Node*>(before->links[0].prev)->data))
	return false;
      
      // For each level, set the next and prev pointers of the new node.
      NodeBase* next = before;
      NodeBase* prev = next->links[0].prev;
      node->links[0].next.set(next);
      node->links[0].prev = prev;
      for (size_t l = 1; l < node->levels; ++l) {
	while (next->levels <= l)
	  next = next->links[l - 1].next.get();
	node->links[l].next.set(next);
	prev = next->links[l].prev;
	node->links[l].prev = prev;
      }

      // Insert the node into the list.
      for (size_t l = 0; l < node->levels; ++l) {
	node->links[l].next.get()->links[l].prev = node;
	// After this line read-only threads can actually see the new node
	// when traversing the list at level l.
	node->links[l].prev->links[l].next.set(node);
      }
      
      return true;
    }
    
    /** Remove the given node from the list. The caller assumes ownership
	of the node. */
    void remove(Node* node) throw() {

      // Change the links of the previous and next nodes to point past
      // the node we're removing, effectively removing it from the list,
      // starting at the top level. Also set the previous links from
      // this node to 0, but don't touch the next links - a read-only
      // thread may be holding a pointer to this node.
      for (int l = node->levels - 1; l >= 0; --l) {
	NodeBase* next = node->links[l].next.get();
	// After this line the read-only threads can no longer see this
	// node when traversing the list at level l.
	node->links[l].prev->links[l].next.set(next);
	next->links[l].prev = node->links[l].prev;
	node->links[l].prev = 0;
      }
    }
    
    
    /** Return the first node with a value not less than @c c, or end_marker()
	if there is no such node. */
    NodeBase* lower_bound(T const& c) {
      return find_less(c)->links[0].next.get();
    }
    

    /** Return the first node with a value not less than @c c, or end_marker()
	if there is no such node. */
    NodeBase const* lower_bound(T const& c) const {
      return find_less(c)->links[0].next.get();
    }
    

    /** Return the first node with a value larger than @c c, or end_marker()
	if there is no such node. */
    NodeBase* upper_bound(T const& c) {
      return find_less_or_equal(c)->links[0].next.get();
    }
    

    /** Return the first node with a value larger than @c c, or end_marker()
	if there is no such node. */
    NodeBase const* upper_bound(T const& c) const {
      return find_less_or_equal(c)->links[0].next.get();
    }
    

    /** Return the last node with a value less than @c c, or head_marker()
	if there is no such node. */
    NodeBase* find_less(T const& c) {
      return find_less_impl(*this, c);
    }
    

    /** Return the last node with a value less than @c c, or head_marker()
	if there is no such node. */
    NodeBase const* find_less(T const& c) const {
      return find_less_impl(*this, c);
    }
    

    /** Return the last node with a value less than or equal to @c c, 
	or head_marker() if there is no such node. */
    NodeBase* find_less_or_equal(T const& c) {
      return find_less_or_equal_impl(*this, c);
    }
    
    
    /** Return the last node with a value less than or equal to @c c, 
	or head_marker() if there is no such node. */
    NodeBase const* find_less_or_equal(T const& c) const {
      return find_less_or_equal_impl(*this, c);
    }
    
  private:
    
    /** A template implementation of find_less(), to avoid duplication of
	code for the const and non-const overloads. */
    template <typename NSL>
    static typename copy_const<NSL, NodeBase>::type* 
    find_less_impl(NSL& me, T const& c) {
      
      typedef typename copy_const<NSL, NodeBase>::type NB;
      typedef typename copy_const<NSL, Node>::type N;
      
      NB* i = &me.m_head;
      int level = M - 1;
      do {
	NB* next = i->links[level].next.get();
	if (next == me.end_marker() || 
	    !(static_cast<N*>(next)->data < c))
	  --level;
	else
	  i = next;
      } while (level >= 0);
      return i;
    }
			     

    /** A template implementation of find_less_or_equal(), to avoid 
	duplication of code for the const and non-const overloads. */
    template <typename NSL>
    static typename copy_const<NSL, NodeBase>::type*
    find_less_or_equal_impl(NSL& me, T const& c) {
      
      typedef typename copy_const<NSL, NodeBase>::type NB;
      typedef typename copy_const<NSL, Node>::type N;
      
      NB* i = &me.m_head;
      int level = M - 1;
      do {
	NB* next = i->links[level].next.get();
	if (next == me.end_marker() || (c < static_cast<N*>(next)->data))
	  --level;
	else
	  i = next;
      } while (level >= 0);
      return i;
    }
			     
    
    /** A pointer to the head of the list. */
    NodeBase m_head;
    
    /** The end marker. */
    NodeBase m_end;
    
  };
  
  
}


#endif
