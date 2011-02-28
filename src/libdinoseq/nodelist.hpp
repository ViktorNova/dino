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

#ifndef NODELIST_HPP
#define NODELIST_HPP

#include <limits>

#include "atomicptr.hpp"


namespace Dino {
  
  
  /** A basic doubly linked list. This is a very basic list, it has no
      iterator interface and the user is responsible for allocating and
      deallocating the Node objects before inserting and after removing
      them, using @c new and @c delete. The only exception is when the
      destructor for the list is called, at which point all nodes still
      in the list will be deallocated using @c delete.
      
      This list type is more suited as a building block for more complex
      data structures than as a stand-alone linked list. All operations
      except construction and destruction are thread-safe and lock-free
      as long as only one thread is calling insert() and remove().
  */
  template <typename T>
  class NodeList {
  public:
    

    /** A base struct for Node. This struct has only a back link and is used 
	for the end marker so we don't have to use Node with its
	potentially expensive data member for that. */
    struct NodeBase {
      
      /** Constructs a new NodeBase. */
      NodeBase(NodeBase* prev = 0) throw() : m_prev(prev) { }
      
      /** A link to the previous node in the list. */
      NodeBase* m_prev;
      
    };
    
    
    /** The node type of NodeList. It inherits NodeBase and adds the data
	member and an AtomicPtr to the next node in the list. */
    struct Node : NodeBase {
      
      /** Constructs a new node with the given data. This function will
	  not throw any exceptions unless the copy constructor for @c T
	  does. */
      Node(T const& data, NodeBase* prev = 0, NodeBase* next = 0) 
	: NodeBase(prev), m_next(next), m_data(data) {
      }
      
      /** Constructs a new node with the given data, which may be moved.
	  This function will not throw any exceptions unless the move
	  constructor for @c T does. */
      Node(T&& data, NodeBase* prev = 0, NodeBase* next = 0) 
	: NodeBase(prev), m_next(next), m_data(std::move(data)) {
      }
      
      /** A pointer to the next node in the list. It is an AtomicPtr instead
	  of a regular pointer to ensure that the list works with multiple
	  threads without requiring locks. */
      AtomicPtr<NodeBase> m_next;
      
      /** The data element of this list node. */
      T m_data;
      
    };
    
    
    /** Construct an empty list. */
    NodeList() throw() 
      : m_head(&m_end) {

    }
    
    /** Release all memory used by the list and its nodes. */
    ~NodeList() throw() {
      NodeBase* nb = m_head.get();
      while (nb != &m_end) {
	Node* n = static_cast<Node*>(nb);
	nb = static_cast<Node*>(nb)->m_next.get();
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
      return m_head.get();
    }
    
    /** Returns the first node in the list. You can use it to insert nodes
	at the beginning using list.insert(list.first_node(), my_node).
	This returns a NodeBase instead of a Node since the list may be
	empty, in which case this function returns the same as end_marker().
	However, if the return value differs from that of end_marker() it is
	safe to use @c static_cast to cast it to a Node pointer. 
    
	This function is atomic and a memory barrier. */
    NodeBase const* first_node() const throw() {
      return m_head.get();
    }
    
    /** Returns a pointer to the end marker of the list. You can use it
	to insert nodes at the end using 
	list.insert(list.end_marker(), my_node). */
    NodeBase* end_marker() throw() {
      return &m_end;
    }
    
    /** Returns a pointer to the end marker of the list. You can use it
	to insert nodes at the end using 
	list.insert(list.end_marker(), my_node). */
    NodeBase const* end_marker() const throw() {
      return &m_end;
    }
    
    /** Insert a new node into the list at a given position. The node will
	be inserted before the NodeBase @c before, which must either be a
	pointer to a node already in the list, or node_end(). The list 
	assumes ownership of the object pointed to by @c node and will 
	deallocate it in the list destructor using @c delete unless the 
	node has been removed from the list before that. This means that 
	the Node object @b must be allocated using @c new unless you are 
	completely sure that it will be removed before the list destructor 
	is called. */
    void insert(NodeBase* before, Node* node) throw() {
      node->m_next.set(before);
      Node* prev = static_cast<Node*>(before->m_prev);
      node->m_prev = prev;
      before->m_prev = node;
      if (prev)
	prev->m_next.set(node);
      else
	m_head.set(node);
    }
    
    /** Remove the given node from the list. The caller assumes ownership
	of the node. */
    void remove(Node* node) throw() {
      Node* prev = static_cast<Node*>(node->m_prev);
      NodeBase* next = node->m_next.get();
      next->m_prev = prev;
      if (prev)
	prev->m_next.set(next);
      else
	m_head.set(next);
    }
    
    
  private:
    
    /** The end marker. */
    NodeBase m_end;
    
    /** A pointer to the head of the list. */
    AtomicPtr<NodeBase> m_head;
    
  };
  
  
}


#endif
