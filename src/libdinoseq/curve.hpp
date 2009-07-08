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

#ifndef CURVE_HPP
#define CURVE_HPP

#include <iterator>
#include <stdexcept>

#include "atomicint.hpp"
#include "meta.hpp"
#include "nodeskiplist.hpp"
#include "sequencable.hpp"
#include "songtime.hpp"


namespace Dino {
  
  
  /** A curve that can be sequenced as MIDI controller values.
      The curve consists of points with specified times and values,
      and when sequenced it will interpolate the events inbetween
      so that the changes in the controller value are reasonably
      smooth. There are functions for adding and removing points,
      as well as moving them around and iterating over them.
      
      @ingroup mididata
  */
  class Curve : public Sequencable {
  public:
    
    /** A point on a curve. */
    struct Point {
      
      /** Create a new Point. */
      Point(SongTime const& st = SongTime(), 
	    AtomicInt::Type v = AtomicInt::Type()) throw();
      
      /** A comparison operator so we can use this as the payload type
	  in a NodeSkipList. */
      bool operator<(Point const& p) const throw();
      
      /** The time of this point, from the start of the curve. */
      SongTime m_time;
      
      /** The value of this point. The full range is always [0, M] where
	  M is @c std::numeric_limits<AtomicInt::Type>::max(), it is then 
	  converted to the actual range of the controller when sequencing. */
      AtomicInt m_value;
    };

  private:
    
    /** The NodeBase type used internally. */
    typedef NodeSkipList<Point>::NodeBase NodeBase;
    
    /** The Node type used internally. */
    typedef NodeSkipList<Point>::Node Node;
    
    
    /** A base class template for Iterator and ConstIterator that
	implements all the common operations.
	@tparam T the Node type (either Node or  Node @c const)
    */
    template <typename Derived, typename Compare, typename N>
    class IteratorT 
      : public std::iterator<std::bidirectional_iterator_tag, Point const> {
    public:
      
      /** The NodeBase type (either NodeBase or NodeBase @c const). */
      typedef typename copy_const<N, NodeBase>::type NB;


      /** Construct a singular iterator. */
      IteratorT() : m_node(0) {}
      
      /** Equality operator. Returns true if @c *this and @c iter point
	  to the same list element. */
      bool operator==(Compare const& iter) const throw() {
	return m_node == iter.m_node;
      }
      
      /** Inequality operator, negation of the equality operator. */
      bool operator!=(Compare const& iter) const throw() {
	return !operator==(iter);
      }
      
      /** Return a proper pointer to the curve point. */
      Point const* operator->() const throw() {
	return &static_cast<N*>(m_node)->data;
      }
      
      /** Return a reference to the curve point. */
      Point const& operator*() const throw() {
	return static_cast<N*>(m_node)->data;
      }
      
      /** Make the iterator point to the next curve point. */
      Derived& operator++() throw() {
	m_node = static_cast<N*>(m_node)->links[0].next.get();
	return static_cast<Derived&>(*this);
      }
      
      /** Make the iterator point to the next element in the list, 
	  postfix version. */
      Derived operator++(int) throw() {
	Derived result = static_cast<Derived&>(*this);
	operator++();
	return result;
      }
      
      /** Make the iterator point to the previous curve point. */
      Derived& operator--() throw() {
	m_node = m_node->links[0].prev;
	return static_cast<Derived&>(*this);
      }
      
      /** Make the iterator point to the previous element in the list, 
	  postfix version. */
      Derived operator--(int) throw() {
	Derived result = static_cast<Derived&>(*this);
	operator--();
	return result;
      }
      
    protected:
      
      /** Create a new IteratorT from a NodeBase pointer. This should only
	  be used by derived classes. */
      IteratorT(NB* node) throw() : m_node(node) {}
      
      /** The NodeBase pointer. */
      NB* m_node;

    };
    
    
  public:
    
    class Iterator;
    
    /** A const bidirectional iterator type over the points of a Curve. */
    class ConstIterator 
      : public IteratorT<ConstIterator, ConstIterator, Node const> {
    public:
      
      /** Create a singular iterator. */
      ConstIterator() throw();
      
    private:
      
      friend class Iterator;
      friend class Curve;
      
      /** Create a new ConstIterator from a NodeBase pointer. */
      ConstIterator(NodeBase const* node) throw();

    };
    
    
    /** A bidirectional iterator over the points of a Curve. This differs
	from ConstIterator in that you can use it with add_point(),
	remove_point() and move_point(). */
    class Iterator : public IteratorT<Iterator, ConstIterator, Node> {
    public:
      
      /** Create a singular iterator. */
      Iterator() throw();
      
      /** Create a ConstIterator from this Iterator. */
      operator ConstIterator() throw();
      
    private:
      
      friend class Curve;
      
      /** Create a new Iterator from a NodeBase pointer. */
      Iterator(NodeBase* node) throw();
      
    };
    
    
    /** XXX This should be moved somewhere else! */
    typedef unsigned ControllerID;
    
    
    /** Create a new Curve with the given label, length and controller ID. */
    Curve(std::string const& label, 
	  SongTime const& length, ControllerID cid = 0) throw();
    
    /** Return the controller ID. */
    ControllerID get_controller_id() const throw();
    
    /** Set the controller ID. */
    void set_controller_id(ControllerID cid) throw();
    
    /** Add a curve point at the last position that keeps the order
	of points consistent. Return an iterator for the new point. 
    
	@throw std::bad_alloc if there isn't enough memory to add the point
	@throw std::out_of_range if @c time is smaller than @c SongTime(0,0)
                                 or larger than get_length()
    */
    Iterator add_point(SongTime const& time, AtomicInt::Type value) 
      throw(std::bad_alloc, std::out_of_range);
    
    /** Add a curve point before the given iterator. Return an iterator for
	the new point, or end() if adding the point in that position would
	make the points unordered. 

	@throw std::bad_alloc if there isn't enough memory to add the point
	@throw std::out_of_range if @c time is smaller than @c SongTime(0,0)
                                 or larger than get_length()
	@throw std::invalid_argument if adding the point at the desired
                                     position would break the order
    */
    Iterator add_point(SongTime const& time, AtomicInt::Type value,
		       Iterator before) 
      throw(std::bad_alloc, std::out_of_range, std::invalid_argument);
    
    /** Move the point referred to by @c iter to the given time and value.
	Return a new iterator to the point (the old one will be invalidated)
	or end() if moving the point to the given time would make the points
	unordered.
	
	@throw std::bad_alloc if there isn't enough memory to add the point
	@throw std::out_of_range if @c time is smaller than @c SongTime(0,0)
                                 or larger than get_length(), or moving the
				 point to the desired time would break the
				 order
    */
    Iterator move_point(Iterator iter, SongTime const& time, 
			AtomicInt::Type value) 
       throw(std::bad_alloc, std::out_of_range);
    
    /** Remove the curve point referred to by @c iter. Return an iterator
	to the next point in the curve. */
    Iterator remove_point(Iterator iter) throw();
    
    /** Return an iterator to the first curve point. */
    Iterator begin() throw();
    
    /** Return an iterator to the end of the curve. This iterator is not
	dereferencable. */
    Iterator end() throw();
    
    /** Return an iterator to the first point in the curve that is not earlier
	than @c time. */
    Iterator lower_bound(SongTime const& time) throw();
    
    /** Return an iterator to the first point in the curve that is later than
	@c time. */
    Iterator upper_bound(SongTime const& time) throw();
    
    /** Return an iterator to the first curve point. */
    ConstIterator begin() const throw();
    
    /** Return an iterator to the end of the curve. This iterator is not
	dereferencable. */
    ConstIterator end() const throw();
    
    /** Return an iterator to the first point in the curve that is not earlier
	than @c time. */
    ConstIterator lower_bound(SongTime const& time) const throw();
    
    /** Return an iterator to the first point in the curve that is later than
	@c time. */
    ConstIterator upper_bound(SongTime const& time) const throw();

    /** Create a new Position object for this sequencable.
	The Position will start at the offset given by @c st. This function
	is @b not realtime safe. */
    virtual std::unique_ptr<Position> 
    create_position(SongTime const& st) const;
    
    /** Update a Position object to a new time. 
	This function is realtime safe and can be called by the sequencer
	in an RT thread. */
    virtual void update_position(Position& pos, SongTime const& st) const;
    
    /** Write MIDI data from the Sequencable to an EventBuffer.
	The function returns @c false if it was not able to write all events
	in the range [@c pos, @c to) to @c buf. @c pos will be updated to point
	to the end of the range of events that were written.
	This function is realtime safe. */
    virtual bool sequence(Position& pos, SongTime const& to, 
			  EventBuffer& buf) const;
    
  private:
    
    /** The list of curve points. */
    NodeSkipList<Point> m_data;
    
    /** The ID of the controller this curve is for. */
    ControllerID m_cid;
    
  };
  

}


#endif
