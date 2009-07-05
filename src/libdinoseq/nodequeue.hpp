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

#ifndef NODEQUEUE_HPP
#define NODEQUEUE_HPP

#include "atomicptr.hpp"


namespace Dino {

  
  /** This is a very basic lock-free realtime-safe queue for one pusher
      and one popper. It does not allocate and deallocate the list nodes
      itself since that would not be realtime-safe, the caller has to do that
      using the ordinary @c new and @c delete operators; @b not @c malloc(), 
      placement @c new or something else, the destructor deallocates all
      remaining nodes using @c delete so they all must be allocated using
      @c new.
      
      This queue has a quirk - pop_node() will return 0 if there is only one
      element in the queue. This means that if you want your pushed node
      to be popped as soon as possible you need to push a dummy node after it.
  */
  template <typename T>
  class NodeQueue {
  public:
    
    /** This is a base class for the queue nodes. It is for internal use only.
     */
    struct NodeBase {
      
      /** Initialise a node with the @c next pointer set to 0. */
      NodeBase() throw() : next(0) {}
      
      /** An atomic pointer to the next element in the queue. */
      AtomicPtr<NodeBase> next;
    };
    
    /** This is the type of object you should allocate (using @c new) and
	push on the queue using push_node(). */
    struct Node : NodeBase {
      
      /** Create a new node with the given data element. This constructor
	  will not throw any exceptions unless the constructor for @c T does.
      */
      Node(T const& d) : data(d) {}
      
      /** The payload. */
      T data;
    };
    
    
    /** Create a new queue. This function is realtime-safe. */
    NodeQueue() throw() : m_tail(&m_head) {}
    
    /** Destroy the queue and delete any elements still in it. This function
	is neither thread-safe nor realtime-safe, so you must make sure that
	no other thread is still pushing or popping when the queue gets 
	destroyed. 
    
	This function will not throw any exceptions unless @b ~T() does. */
    ~NodeQueue() {
      NodeBase* nb = m_head.next.get();
      while (nb != 0) {
	Node* n = static_cast<Node*>(nb);
	nb = nb->next.get();
	delete n;
      }
    }
    
    /** Push a new node onto the end of the queue. This function may only be
	called from one single thread. The @c node must not already be in the
	queue. 
    
	This function is realtime-safe. */
    void push_node(Node* node) throw() {
      // this function never touches any node other than the tail
      node->next.set(0);
      m_tail->next.set(node);
      m_tail = node;
    }
    
    /** Pop a node from the beginning of the queue. This function may only be
	called from one single thread. It will return 0 if there are less than
	2 nodes in the queue, the last node will not be popped until more
	nodes have been pushed. Once you have popped a node you are responisble
	for deallocating it using @c delete.
	
	This function is realtime-safe. */
    Node* pop_node() {
      // if this is 0 the queue is empty
      NodeBase* nb = m_head.next.get();
      if (nb == 0)
	return 0;
      
      // if this is 0 the queue contains one single element (the tail), and 
      // we can't pop it since push_node() may be writing to it
      NodeBase* nb2 = nb->next.get();
      if (nb2 == 0)
	return 0;
      
      // else, there is more than one element in the queue and we can pop the
      // first one since push() only ever writes to the tail
      m_head.next.set(nb2);
      return static_cast<Node*>(nb);
    }
    
  private:
    
    /** The head of the queue. This node never gets popped, it's just a 
	marker. */
    NodeBase m_head;
    
    /** A pointer to the end of the queue, where new elements get pushed. */
    NodeBase* m_tail;
    
  };
  

}


#endif
