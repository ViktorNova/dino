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

#include "curve.hpp"


namespace Dino {
  
  
  using std::bad_alloc;
  using std::invalid_argument;
  using std::out_of_range;
  using std::shared_ptr;
  using std::string;
  using std::unique_ptr;
  

  Curve::Point::Point(SongTime const& st, AtomicInt::Type v) throw()
    : m_time(st),
      m_value(v) {
  }


  bool Curve::Point::operator<(Point const& p) const throw() {
    return m_time < p.m_time;
  }
  
  
  Curve::ConstIterator::ConstIterator() throw() 
    : IteratorT<ConstIterator, ConstIterator, Node const>(0) {
  }


  Curve::ConstIterator::ConstIterator(NodeBase const* node) throw()
    : IteratorT<ConstIterator, ConstIterator, Node const>(node) {
  }

  
  Curve::Iterator::Iterator() throw() 
    : IteratorT<Iterator, ConstIterator, Node>(0) {}


  Curve::Iterator::operator ConstIterator() throw() {
    return ConstIterator(m_node);
  }

  
  Curve::Iterator::Iterator(NodeBase* node) throw() 
    : IteratorT<Iterator, ConstIterator, Node>(node) {}
    
    
  Curve::Curve(string const& label, 
	       SongTime const& length, ControllerID cid) throw() 
    : Sequencable(label, length),
      m_cid(cid) {
  }
  
  
  Curve::~Curve() throw() {
    for (auto i = m_positions.begin(); i != m_positions.end(); ++i)
      (*i)->curve = 0;
  }
    
  
  Curve::ControllerID Curve::get_controller_id() const throw() {
    return m_cid;
  }
  
  
  void Curve::set_controller_id(ControllerID cid) throw() {
    m_cid = cid;
  }
    
  
  Curve::Iterator Curve::add_point(SongTime const& time, AtomicInt::Type value)
    throw(bad_alloc, out_of_range) {

    // check that the time given isn't out of range
    if (time > get_length() || time < SongTime(0, 0))
      throw out_of_range("Time for curve point is out of range");
    
    Node* n = new Node(Point(time, value));
    Iterator i = upper_bound(time);
    m_data.insert(i.m_node, n);
    return Iterator(n);
  }
  
  
  Curve::Iterator Curve::add_point(SongTime const& time, AtomicInt::Type value,
				   Iterator before)
    throw(bad_alloc, out_of_range, invalid_argument) {
    
    // First, delete any old nodes that should be deleted.
    delete_queued_nodes();
    
    // check that the time given isn't out of range
    if (time > get_length() || time < SongTime(0, 0))
      throw out_of_range("Time for curve point is out of range");
    
    // check that we are inserting the point at a valid position
    Iterator tmp = before;
    if (!((before == end() || before->m_time >= time) &&
	  (before == begin() || (--tmp)->m_time <= time)))
      throw invalid_argument("Inserting the point at the given position would "
			     "break the order");
    
    Node* n = new Node(Point(time, value));
    m_data.insert(before.m_node, n);
    return Iterator(n);
  }
  
  
  Curve::Iterator Curve::move_point(Iterator iter, SongTime const& time, 
				    AtomicInt::Type value)
    throw(bad_alloc, out_of_range) {
    // First, remove any old nodes that need to be deleted.
    delete_queued_nodes();
    
    /* We could use remove_point(iter); add_point(time, value, ++iter) here,
       but then we would have to re-add the old point if the new time was out
       of range and the less we modify the data the better. */
    
    // Check that the time given isn't out of range.
    if (time != iter->m_time) {
      bool oor = false;
      Iterator tmp1 = iter;
      Iterator tmp2 = iter;
      if (iter == begin())
	oor = oor || time < SongTime(0, 0);
      else
	oor = oor || time < (--tmp1)->m_time;
      if (!oor) {
	if (++tmp2 == end())
	  oor = oor || time > get_length();
	else
	  oor = oor || time > tmp2->m_time;
      }
      if (oor)
	throw out_of_range("New time for curve point is out of range or "
			   "would break the order");
    }
    
    // If the time has changed we need to remove the node and add a new one.
    if (time != iter->m_time) {
      Node* n = new Node(Point(time, value));
      Iterator before = iter;
      m_data.insert((++before).m_node, n);
      Node* old = static_cast<Node*>(iter.m_node);
      m_data.remove(old);
      shared_ptr<Node> sp = shared_ptr<Node>(old);
      for (auto i = m_positions.begin(); i != m_positions.end(); ++i) {
	(*i)->to_be_confirmed.
	  push_node(new NodeQueue<shared_ptr<Node>>::Node(sp));
      }
      return Iterator(n);
    }
    
    // If not we can just tweak the value.
    static_cast<Node*>(iter.m_node)->data.m_value.set(value);
    return iter;
  }
  
  
  Curve::Iterator Curve::remove_point(Iterator iter) throw() {
    // First, delete any old nodes that should be deleted.
    delete_queued_nodes();
    
    // Then, remove this node.
    Iterator next = iter;
    ++next;
    Node* node = static_cast<Node*>(iter.m_node);
    m_data.remove(node);
    shared_ptr<Node> sp = shared_ptr<Node>(node);
    for (auto i = m_positions.begin(); i != m_positions.end(); ++i) {
      (*i)->to_be_confirmed.
	push_node(new NodeQueue<shared_ptr<Node>>::Node(sp));
    }
    return next;
  }
    
  
  Curve::Iterator Curve::begin() throw() {
    return Iterator(m_data.first_node());
  }
  
  
  Curve::Iterator Curve::end() throw() {
    return Iterator(m_data.end_marker());
  }
    
  
  Curve::Iterator Curve::lower_bound(SongTime const& time) throw() {
    return Iterator(m_data.lower_bound(Point(time)));
  }
  

  Curve::Iterator Curve::upper_bound(SongTime const& time) throw() {
    return Iterator(m_data.upper_bound(Point(time)));
  }
    
   
  Curve::ConstIterator Curve::begin() const throw() {
    return ConstIterator(m_data.first_node());
  }
  
    
  Curve::ConstIterator Curve::end() const throw() {
    return ConstIterator(m_data.end_marker());
  }
    
  
  Curve::ConstIterator Curve::lower_bound(SongTime const& time) const throw() {
    return ConstIterator(m_data.lower_bound(Point(time)));
  }
  
  
  Curve::ConstIterator Curve::upper_bound(SongTime const& time) const throw() {
    return ConstIterator(m_data.upper_bound(Point(time)));
  }
  
  
  unique_ptr<Sequencable::Position> 
  Curve::create_position(SongTime const& st) const {
    auto pos = unique_ptr<CurvePosition>(new CurvePosition());
    update_position(*pos, st);
    return move(pos);
  }
    
  
  void Curve::update_position(Sequencable::Position& pos, 
			      SongTime const& st) const {
    Sequencable::update_position(pos, st);
    static_cast<CurvePosition&>(pos).node = m_data.find_less(Point(st));
  }
  
  
  bool Curve::sequence(Sequencable::Position& pos, SongTime const& to, 
		       EventBuffer& /*buf*/) const {
    // check if the position needs to be updated
    CurvePosition& cp = static_cast<CurvePosition&>(pos);
    NodeQueue<shared_ptr<Node>>::Node* n;
    bool needs_update = false;
    while ((n = cp.to_be_confirmed.pop_node())) {
      if (n->data.get() == cp.node)
	needs_update = true;
      cp.to_be_deleted.push_node(n);
    }
    if (needs_update)
      update_position(pos, pos.get_time());
    
    // for each curve point with time < to:
    NodeBase* nb = cp.node->links[0].next.get();
    NodeBase* last_sequenced = nb;
    while (nb != m_data.end_marker()) {
      Node* node = static_cast<Node*>(node);
      if (node->data.m_time >= to)
	break;
      last_sequenced = node;
      if (node->data.m_time >= cp.get_time()) {
	//   interpolate events until to and write to buf
      }
      nb = node->links[0].next.get();
    }
    
    // update pos with time to and the last node before it
    Sequencable::update_position(pos, to);
    cp.node = last_sequenced;
    
    return true;
  }


  void Curve::remove_curve_position(CurvePosition* c) {
    auto iter = m_positions.find(c);
    if (iter != m_positions.end())
      m_positions.erase(iter);
  }


  void Curve::delete_queued_nodes() throw() {
    for (auto i = m_positions.begin(); i != m_positions.end(); ++i) {
      NodeQueue<shared_ptr<Node>>::Node* n;
      while ((n = (*i)->to_be_deleted.pop_node()))
	delete n;
    }
  }


}
