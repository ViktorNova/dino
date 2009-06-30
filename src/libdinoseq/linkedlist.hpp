/*****************************************************************************
    libdinoseq - a library for MIDI sequencing
    Copyright (C) 2009  Lars Luthman <lars.luthman@gmail.com>
    
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

#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include <limits>
#include <stdexcept>

#include "atomicint.hpp"
#include "atomicptr.hpp"


namespace Dino {
  
  
  /** A lock-free linked list with one writer and one reader.
      This class template implements a lock-free linked list for one RW thread
      and one read-only thread, the latter of which may be a realtime thread.
      
      The RW thread should do all the manipulation of the list using insert()
      and erase(), and can also iterate over the list using the iterators
      returned by the begin() and end() member functions. 
      
      The read-only thread should only use reader_begin() and reader_end()
      to iterate over the list, as well as calling 
      reader_holds_no_iterator() when the reader thread does not hold any
      ReaderIterator objects for this LinkedList. These functions are realtime
      safe.
      
      No functions take any locks, however insert(), erase() and 
      delete_erased_nodes() may allocate or deallocate memory and should thus
      not be called from a realtime thread. 
  
      The only requirement for the datatype T is that is should be 
      CopyConstructable or MoveConstructable. */
  template <typename T>
  class LinkedList {
  private:
    
    
    /** A base struct for Node. This struct has only a back link and is used 
	for the end marker so we don't have to use Node with its
	potentially expensive data member for that. */
    struct NodeBase {
      
      /** Constructs a new NodeBase. */
      NodeBase(NodeBase* prev = 0) throw() : m_prev(prev) { }
      
      /** A link to the previous node in the list. */
      NodeBase* m_prev;
      
    };
    
    
    /** The node type of LinkedList. It inherits NodeBase and adds the data
	member and an AtomicPtr to the next node in the list. */
    struct Node : NodeBase {
      
      /** Constructs a new node with the given previous node, next node and
	  data member. */
      Node(NodeBase* prev, NodeBase* next, T const& data) 
	: NodeBase(prev), m_next(next), m_data(data) { }
      
      /** Constructs a new node with the given previous node, next node and
	  data member, which may be moved. */
      Node(NodeBase* prev, NodeBase* next, T&& data) 
	: NodeBase(prev), m_next(next), m_data(std::move(data)) { }
      
      /** A pointer to the next node in the list. It is an AtomicPtr instead
	  of a regular pointer to ensure that the list works with two threads
	  without requiring locks. */
      AtomicPtr<NodeBase> m_next;
      
      /** The data element of this list node. */
      T m_data;
      
    };
    

    /** A class template that implements all the operations of a
	ForwardIterator for the list. */
    template <typename S, typename C, typename V, typename N, typename NB>
    class ForwardIterator {
    protected:
      
      /** The constructor that creates an iterator from a list node. */
      explicit ForwardIterator(NB* node) throw() : m_node(node) { }
      
    public:
      
      /** Default constructor, creates a singular iterator. */
      ForwardIterator() throw() : m_node(0) { }
      
      /** Equality operator. Returns true if @c *this and @c iter point
	  to the same list element. */
      bool operator==(C const& iter) const throw() {
	return m_node == iter.m_node;
      }
      
      /** Inequality operator, negation of the equality operator. */
      bool operator!=(C const& iter) const throw() {
	return !operator==(iter);
      }
      
      /** Return a proper pointer to the list element. */
      V* operator->() const throw() { 
	return &static_cast<N*>(m_node)->m_data; 
      }
      
      /** Return a reference to the list element. */
      V& operator*() const throw() { 
	return static_cast<N*>(m_node)->m_data; 
      }
      
      /** Make the iterator point to the next element in the list. */
      S& operator++() throw() {
	m_node = static_cast<N*>(m_node)->m_next.get();
	return static_cast<S&>(*this);
      }
      
      /** Make the iterator point to the next element in the list, 
	  postfix version. */
      S operator++(int) throw() {
	S tmp(m_node);
	operator++();
	return tmp;
      }
      
    protected:
      
      /** A pointer to the list node of the element this iterator points to. */
      NB* m_node;
      
    };
    
    
    /** This class template inherits ForwardIterator and adds decrement 
	operators. */
    template <typename S, typename C, typename V, typename N, typename NB>
    class BiIterator : public ForwardIterator<S, C, V, N, NB> {
    protected:
      
      /** LinkedList<T> needs to call the private constructor. */
      friend class LinkedList<T>;
      
      /** The constructor that creates an iterator from a list node. */
      explicit BiIterator(NB* node) throw() 
	: ForwardIterator<S, C, V, N, NB>(node) { 
      }
      
    public:
      
      /** Default constructor. Creates a singular iterator. */
      BiIterator() throw() { }
      
      /** Make the iterator point to the previous element in the list. */
      S& operator--() throw() {
	this->m_node = this->m_node->m_prev;
	return static_cast<S&>(*this);
      }
      
      /** Make the iterator point to the previous element in the list,
	  postfix version. */
      S operator--(int) throw() {
	S tmp(this->m_node);
	operator--();
	return tmp;
      }
      
    };
    
  public:
    
    /** A bidirectional non-mutable iterator for the list. Should only be used
	in the RW thread. */
    class ConstIterator 
      : public BiIterator<ConstIterator, ConstIterator, T const, 
			  Node const, NodeBase const>,
	public std::iterator<std::bidirectional_iterator_tag, 
			     T const, AtomicInt::Type> {

      /** LinkedList<T> needs to call the private constructor. */
      friend class LinkedList<T>;
      
      /** The constructor that creates an iterator from a list node. */
      explicit ConstIterator(NodeBase const* node) throw() 
	: BiIterator<ConstIterator, ConstIterator, T const, 
		     Node const, NodeBase const>(node) { 
      }
      
    public:
      
      /** The default constructor creates a singular iterator. */
      ConstIterator() throw() { }
      
    };
    
    
    /** A bidirectional mutable iterator for the list. Should only be used
	in the RW thread. */
    class Iterator 
      : public BiIterator<Iterator, ConstIterator, T, Node, NodeBase>, 
	public std::iterator<std::bidirectional_iterator_tag, 
			     T, AtomicInt::Type> {
      
      /** LinkedList<T> needs to call the private constructor. */
      friend class LinkedList<T>;
      
      /** The constructor that creates an iterator from a list node. */
      explicit Iterator(NodeBase* node) throw() 
	: BiIterator<Iterator, ConstIterator, T, Node, NodeBase>(node) { 
      }
      
    public:
      
      /** The default constructor creates a singular iterator. */
      Iterator() throw() { }

      /** A cast operator for converting to ConstIterator. */
      operator ConstIterator() const throw() {
	return ConstIterator(this->m_node);
      }
      
    };

    
    /** A non-mutable forward iterator for the list. This may be used in both
	the reader thread and the RW thread. */
    class ReaderIterator
      : public ForwardIterator<ReaderIterator, ReaderIterator, T const, 
			       Node const, NodeBase const>,
	public std::iterator<std::forward_iterator_tag, T, AtomicInt::Type> {

      /** LinkedList<T> needs to call the private constructor. */
      friend class LinkedList<T>;
      
      /** The constructor that creates an iterator from a list node. */
      explicit ReaderIterator(NodeBase const* node) throw() 
	: ForwardIterator<ReaderIterator, ReaderIterator, T const, 
			  Node const, NodeBase const>(node) {
      }
      
    public:
    
      /** The default constructor creates a singular iterator. */
      ReaderIterator() throw() { }
    
    };
    
    
    /** Construct an empty list. */
    LinkedList() throw() 
      : m_head(&m_end), 
	m_erased_list(0),
	m_erase_counter(0),
	m_delete_ok(std::numeric_limits<decltype(m_delete_ok)>::max()),
	m_size(0),
	m_erased_list_size(0) {

    }
    
    /** Returns a ConstIterator to the beginning of the list. */
    ConstIterator begin() const throw() {
      return ConstIterator(m_head.get());
    }
    
    /** Returns a ConstIterator to the end of the list. */
    ConstIterator end() const throw() {
      return ConstIterator(&m_end);
    }
    
    /** Returns an Iterator to the beginning of the list. */
    Iterator begin() throw() {
      return Iterator(m_head.get());
    }
    
    /** Returns an Iterator to the end of the list. */
    Iterator end() throw() {
      return Iterator(&m_end);
    }
    
    /** Copy @c data into the list before the element pointed to by
	@c pos.
	
	@throw std::bad_alloc if the memory for the list node can't be 
	                      allocated
	@throw std::overflow_error if the list already has the maximal number
	                           of elements. */
    Iterator insert(Iterator pos, T const& data) throw(std::bad_alloc,
						       std::overflow_error) {
      delete_erased_nodes();
      if (m_size == std::numeric_limits<AtomicInt::Type>::max())
	throw std::overflow_error("The list is full");
      Node* prev = static_cast<Node*>(pos.m_node->m_prev);
      Node* node = new Node(prev, pos.m_node, data);
      pos.m_node->m_prev = node;
      if (prev)
	prev->m_next.set(node);
      else
	m_head.set(node);
      ++m_size;
      return Iterator(node);
    }
    

    /** Move @c data into the list before the element pointed to by
	@c pos.
	
	@throw std::bad_alloc if the memory for the list node can't be 
	                      allocated
	@throw std::overflow_error if the list already has the maximal number
	                           of elements. */
    Iterator insert(Iterator pos, T&& data) throw(std::bad_alloc,
						  std::overflow_error) {
      delete_erased_nodes();
      if (m_size == std::numeric_limits<AtomicInt::Type>::max())
	throw std::overflow_error("The list is full");
      Node* prev = static_cast<Node*>(pos.m_node->m_prev);
      Node* node = new Node(prev, pos.m_node, std::move(data));
      pos.m_node->m_prev = node;
      if (prev)
	prev->m_next.set(node);
      else
	m_head.set(node);
      ++m_size;
      return Iterator(node);
    }

    
    /** Erase the element pointed to by @c pos from the list.
	@throw std::overflow_error if there are too many erased elements that
	                           have not been deallocated yet. See 
				   reader_holds_no_iterator(). */
    Iterator erase(Iterator pos) throw(std::overflow_error) {
      delete_erased_nodes();
      if (m_erased_list_size == std::numeric_limits<AtomicInt::Type>::max())
	throw std::overflow_error("Too many elements waiting for deletion");
      Iterator result = pos;
      ++result;
      Node* node = static_cast<Node*>(pos.m_node);
      Node* prev = static_cast<Node*>(node->m_prev);
      NodeBase* next = node->m_next.get();
      next->m_prev = prev;
      if (prev)
	prev->m_next.set(next);
      else
	m_head.set(next);
      node->m_prev = m_erased_list;
      ++m_erased_list_size;
      m_erase_counter.increase();
      --m_size;
      return result;
    }
    
    /** Deallocate all erased nodes. See reader_holds_no_iterator(). This is
	called automatically when you insert() or erase() so you should 
	normally not have to call it directly. */
    AtomicInt::Type delete_erased_nodes() throw() {
      if (m_erase_counter.get() == m_delete_ok.get()) {
	Node* node;
	while ((node = m_erased_list)) {
	  delete m_erased_list;
	  m_erased_list = static_cast<Node*>(node->m_prev);
	}
	AtomicInt::Type result = m_erased_list_size;
	m_erased_list_size = 0;
	return result;
      }
      return 0;
    }
    
    /** Returns the number of elements in the list. This should not be
	called from the read-only thread. */
    AtomicInt::Type get_size() const throw() {
      return m_size;
    }
    
    
    /** @name Read-only thread
	These functions are the only ones that should be called in the
	read-only thread. 
	@{ */
    
    /** Returns a ReaderIterator to the beginning of the list. */
    ReaderIterator reader_begin() const throw() {
      return ReaderIterator(m_head.get());
    }
    
    /** Returns a ReaderIterator to the end of the list. */
    ReaderIterator reader_end() const throw() {
      return ReaderIterator(&m_end);
    }
    
    /** Tell the list that it's OK to deallocate all erased nodes.
	When the RW thread calls erase() the node for that element can not
	be deallocated immediately since the read-only thread may be doing
	something with it through a ReaderIterator. Thus the node is queued,
	and will not be deleted until after a subsequent call to this function
	followed by a call to insert(), erase() or delete_erased_nodes() in
	the RW thread.
	
	The reader thread should call this function periodically when it does
	not hold any iterators for the list. After a call to this function
	any ReaderIterator returned from an earlier reader_begin() or 
	reader_end() may be invalid. */
    void reader_holds_no_iterator() throw() {
      m_delete_ok.set(m_erase_counter.get());
    }
    
    /** @} */
    
  private:
    
    /** The end marker. */
    NodeBase m_end;
    
    /** A pointer to the head of the list. */
    AtomicPtr<NodeBase> m_head;
    
    /** A list of erased nodes waiting for deallocation, linked through the
	@c m_prev link to save atomic calls. */
    Node* m_erased_list;
    
    /** A counter that is increased every time erase() is called. */
    AtomicInt m_erase_counter;
    
    /** reader_holds_no_iterator() copies the value of AtomicInt here.
	delete_erased_nodes() will only actually delete nodes if the values
	of @c m_delete_ok and @c m_erase_counter are equal. */
    AtomicInt m_delete_ok;
    
    /** The number of elements in the list. */
    AtomicInt::Type m_size;
    
    /** The number of nodes in the list of erased nodes. */
    AtomicInt::Type m_erased_list_size;
    
  };
  
  
}


#endif
