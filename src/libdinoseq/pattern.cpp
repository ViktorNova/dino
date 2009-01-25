/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include <sys/types.h>
#include <signal.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <set>

#include "controller_numbers.hpp"
#include "controllerinfo.hpp"
#include "curve.hpp"
#include "debug.hpp"
#include "deleter.hpp"
#include "interpolatedevent.hpp"
#include "midibuffer.hpp"
#include "note.hpp"
#include "notecollection.hpp"
#include "noteevent.hpp"
#include "pattern.hpp"
#include "noteselection.hpp"


using namespace std;
using namespace xmlpp;


namespace Dino {
  
  
  Pattern::NoteIterator::NoteIterator() 
    : m_pattern(0),
      m_event(0) {

  }
  
  
  Pattern::NoteIterator::NoteIterator(const Pattern* pat, Event* event)
    : m_pattern(pat),
      m_event(event) {

  }
  
  
  Event& Pattern::NoteIterator::operator*() const {
    return *(m_event);
  }
  
  
  Event* Pattern::NoteIterator::operator->() const {
    return (m_event);
  }
  
  
  bool Pattern::NoteIterator::operator==(const NoteIterator& iter) const {
    return (m_pattern == iter.m_pattern && m_event == iter.m_event);
  }
  
  
  bool Pattern::NoteIterator::operator!=(const NoteIterator& iter) const {
    return !operator==(iter);
  }
  
  
  bool Pattern::NoteIterator::operator<(const NoteIterator& iter) const {
    return (m_event->get_time() < iter->get_time()) || 
      ((m_event->get_time() == iter->get_time()) && 
       (m_event->get_key() < iter->get_key()));
  }


  Pattern::NoteIterator& Pattern::NoteIterator::operator++() {
    
    for ( ; m_event; m_event = m_event->m_next[0]) {
      if (Event::is_note_on(m_event->get_type()))
	break;
    }

    return *this;
  }


  Pattern::NoteIterator Pattern::NoteIterator::operator++(int) {
    NoteIterator result = *this;
    ++(*this);
    return result;
  }

  
  Pattern::Pattern(int id, const string& name, 
		   const SongTime& length, int steps) 
    : m_id(id),
      m_name(name),
      m_dirty(false),
      m_length(length) {
    
    dbg1<<"Creating pattern \""<<m_name<<"\""<<endl;
    
    assert(steps > 0);
    assert(length > SongTime(0, 0));
    
    m_min_step = length.get_beat() * steps;
    m_max_step = -1;
    m_min_note = 128;
    m_max_note = -1;
  }


  Pattern::Pattern(int id, const Pattern& pat) 
    : m_id(id),
      m_name(pat.get_name()),
      m_dirty(false),
      m_length(pat.m_length) {

    dbg1<<"Duplicating pattern \""<<pat.get_name()<<"\""<<endl;
    
    m_min_step = 9999999;
    m_max_step = -1;
    m_min_note = 128;
    m_max_note = -1;
    
    // copy all notes
    NoteIterator iter;
    for (iter = pat.notes_begin(); iter != pat.notes_end(); ++iter) {
      add_note(iter->get_time(), iter->get_key(), 
               iter->get_velocity(), iter->get_length());
    }
    
    // copy all curves with data
    // XXX fix this!
    /*CurveIterator citer;
    for (citer = pat.curves_begin(); citer != pat.curves_end(); ++citer) {
      CurveIterator nctrl = add_controller(citer->get_name(),
					   citer->get_param(),
					   citer->get_min(), 
					   citer->get_max());
      for (unsigned i = 0; i < citer->get_size(); ++i) {
        const InterpolatedEvent* evt = citer->get_event(i);
        if (evt && evt->get_step() == i)
          add_cc(nctrl, i, evt->get_start());
      }
      if (nctrl->get_event(nctrl->get_size() - 1))
        add_cc(nctrl, nctrl->get_size(), 
               citer->get_event(nctrl->get_size() - 1)->get_end());
    }
    */
  }


  Pattern::~Pattern() {
    dbg1<<"Destroying pattern \""<<m_name<<"\""<<endl;
    NoteEventList::iterator iter;
    
    // delete all note on events
    
    // delete all note off events
    
    // delete all containers

  }
  
  
  int Pattern::get_id() const {
    return m_id;
  }
  

  const string& Pattern::get_name() const {
    return m_name;
  }


  const string& Pattern::get_label() const {
    return m_name;
  }


  Pattern::NoteIterator Pattern::notes_begin() const {
    // XXX clean this up!
    Event* e = const_cast<EventList&>(m_events).get_start();
    while (e && Event::is_note_on(e->get_type()))
      e = e->m_next[0];
    return NoteIterator(this, e);
  }

  
  Pattern::NoteIterator Pattern::notes_end() const {
    return NoteIterator(this, 0);
  }

  
  void Pattern::set_name(const string& name) {
    if (name != m_name) {
      dbg1<<"Changing pattern name from \""<<m_name<<"\" to \""
          <<name<<"\""<<endl;
      m_name = name;
      m_signal_name_changed(m_name);
    }
  }


  bool Pattern::set_length(const SongTime& length) {
    
    // can't set a negative or zero length
    if (length <= SongTime(0, 0))
      return false;
    
    // no change?
    if (length == m_length)
      return true;
    
    // if the new length is shorter we may have to delete and resize notes
    if (length < m_length) {
      NoteIterator iter = notes_begin();
      NoteIterator iter2;
      while (iter != notes_end()) {
	if (iter->get_time() >= length) {
	  iter2 = iter;
	  ++iter;
	  delete_note(&*iter2);
	}
	else if (iter->get_time() + iter->get_length() > length)
	  resize_note(&*iter, length - iter->get_time());
	++iter;
      }
    }
    
    m_length = length;
    m_signal_length_changed(m_length);
  }


  void Pattern::set_steps(unsigned int steps) {
    assert(steps > 0);
    
    /*
    
    if (steps == m_sd->steps)
      return;
    
    // allocate new data structures
    NoteEventList* new_note_ons = 
      new NoteEventList(steps * m_sd->length.get_beat());
    NoteEventList* new_note_offs = 
      new NoteEventList(steps * m_sd->length.get_beat());
    vector<Curve*>* new_controllers = new vector<Curve*>();
    
    // copy information about the notes
    NoteIterator iter;
    vector<int> starts;
    vector<int> lengths;
    vector<int> keys;
    vector<int> velocities;
    for (iter = notes_begin(); iter != notes_end(); ) {
      starts.push_back(int(steps * iter->get_step() / double(m_sd->steps))); 
      lengths.push_back(int(steps * iter->get_length() / double(m_sd->steps))); 
      keys.push_back(iter->get_key());
      velocities.push_back(iter->get_velocity());
      NoteIterator diter = iter;
      ++iter;
      delete_note(diter);
    }
    
    
    // iterate over all controllers in the old controller list
    for (unsigned i = 0; i < m_sd->curves->size(); ++i) {
      Curve* c = (*m_sd->curves)[i];
      
      // create a new controller with the same settings but different size
      Curve* new_c = new Curve(c->get_info(), m_sd->length.get_beat() * steps);
      new_controllers->push_back(new_c);
      
      // copy the events from the old controller to the new controller
      // this may not be a 1 to 1 copy since the step resolutions differ
      for (unsigned j = 0; j < m_sd->length.get_beat() * m_sd->steps; ++j) {
        const InterpolatedEvent* e = c->get_event(j);
        if (e != 0) {
          new_c->add_point(unsigned(steps * j / double(m_sd->steps)), 
                           e->get_start());
        }
      }
    }
    
    // swap the SeqData pointer
    SeqData* old_sd = m_sd;
    m_sd = new SeqData(new_note_ons, new_note_offs, new_controllers, 
                       old_sd->length, steps);
    Deleter::queue(old_sd);
    
    // add the notes using the new event lists
    for (unsigned i = 0; i < starts.size(); ++i)
      add_note(SongTime(starts[i], 0), keys[i], velocities[i], 
	       SongTime(lengths[i], 0));
    
    m_signal_steps_changed(m_sd->length.get_beat());
    
    */
  }


  /** This function tries to add a new note with the given parameters. If 
      there is another note starting at the same step and with the same value,
      the old note will be deleted. If there is a note that is playing at that
      step with the same value, the old note will be shortened so it ends just
      before @c step. 
  */
  Pattern::NoteIterator Pattern::add_note(const SongTime& start, int key, 
					  int velocity, 
					  const SongTime& length) {
    // sanity checks
    assert(start > SongTime(0, 0));
    assert(start < m_length);
    assert(length > SongTime(0, 0));
    assert(start + length <= m_length);
    assert(key < 128);
    assert(velocity < 128);
    
    // check that the new note won't overlap with any old ones
    if (!check_free_space(start, key, length))
      return notes_end();
    
    // create the events
    Event* note_on = new Event(Event::note_on(), start, key, velocity);
    Event* note_off = new Event(Event::note_off(), start + length, key, 64);
    note_on->set_buddy(note_off);
    note_off->set_buddy(note_on);

    // insert the events in the list
    Event* node = m_events.insert(note_off);
    if (node)
      node = m_events.insert(note_on);
    
    return NoteIterator(this, node);
  }
  
  
  bool Pattern::add_notes(const NoteCollection& notes, const SongTime& start,
			  int key, NoteSelection* selection) {
    
    // sanity checks
    assert(start < m_length);
    assert(key < 128);
    
    // clear the selection if there is one
    if (selection)
      selection->clear();
    
    // check if we can add all the notes in the collection, otherwise don't
    // try to add any of them
    NoteCollection::ConstIterator iter;
    for (iter = notes.begin(); iter != notes.end(); ++iter) {
      if (int(iter->key) + int(key) > 127)
	return false;
      if (!check_free_space(start + iter->start, iter->key + key,
			    iter->length))
	return false;
    }

    // actually add the notes
    for (iter = notes.begin(); iter != notes.end(); ++iter) {
      NoteIterator iter2 = add_note(start + iter->start, 
				    iter->key + key, 
                                    iter->velocity, 
				    iter->length);
      if (selection)
        selection->add_note(iter2);
    }
    
    return true;
  }


  void Pattern::delete_note(NoteIterator iterator) {
    assert(iterator != notes_end());
    assert(iterator.m_pattern == this);
    delete_note(iterator.m_event);
  }


  void Pattern::delete_note(Event* note_on) {

    assert(note_on);

    Event* note_off = note_on->get_buddy();
    
    assert(note_off);
    
    m_events.erase(note_off);
    m_events.erase(note_on);
    
    // delete the note object and queue the events for deletion
    m_signal_note_removed(*note_on);
    Deleter::queue(note_off);
    Deleter::queue(note_on);
  }


  SongTime Pattern::resize_note(NoteIterator iterator, 
				const SongTime& length) {
    
    assert(iterator != notes_end());
    assert(iterator.m_pattern == this);
    
    SongTime result = resize_note(iterator.m_event, length);
    
    m_signal_note_changed(*iterator.m_event);
    
    return result;
  }


  SongTime Pattern::resize_note(Event* note, const SongTime& length) {
    
    // same length?
    if (note->get_length() == length)
      return length;
    
    // check if there is room for a note of the wanted length or of it has to
    // be shortened
    SongTime new_length = length;
    if (length > note->get_length()) {
      NoteIterator iter(this, note);
      ++iter;
      for ( ; iter != notes_end(); ++iter) {
	if (iter->get_time() >= note->get_time() + length)
	  break;
	if (iter->get_key() == note->get_key()) {
	  new_length = iter->get_time() - note->get_time();
	  break;
	}
      }
      if (note->get_time() + new_length > m_length)
	new_length = m_length - note->get_time();
    }
    
    // create the new note off event, insert it and remove the old one
    Event* old_off = note->get_buddy();
    Event* new_off = new Event(Event::note_off(), note->get_time() + length,
			       old_off->get_key(), old_off->get_velocity());
    new_off->m_buddy = note;
    if (!m_events.insert(new_off)) {
      delete new_off;
      return note->get_length();
    }
    note->m_buddy = new_off;
    m_events.erase(old_off);
    Deleter::queue(old_off);
    
    return new_length;
  }


  void Pattern::set_velocity(NoteIterator iterator, unsigned char velocity) {
    assert(iterator != notes_end());
    assert(iterator.m_pattern == this);
    assert(velocity < 128);
    
    iterator->set_velocity(velocity);
    
    m_signal_note_changed(*iterator);
  }

  
  /*
  Pattern::CurveIterator Pattern::add_controller(const std::string& name,
						 long param, 
						 int min, int max) {
#if 0
    // find the place to insert the new controller
    unsigned i;
    for (i = 0; i < m_sd->curves->size(); ++i) {
      long this_param = (*m_sd->curves)[i]->get_param();
      if (this_param == param)
        return ControllerIterator(m_sd->curves->begin() + i);
      else if (this_param > param)
        break;
    }
    
    // make a copy of the controller vector and insert the new one
    vector<Curve*>* new_vector = new vector<Curve*>(*m_sd->curves);
    new_vector->insert(new_vector->begin() + i, 
                       new Curve(name, m_sd->steps * m_sd->length, 
				 param, min, max));
    
    // delete the old vector
    vector<Curve*>* tmp = m_sd->curves;
    m_sd->curves = new_vector;
    Deleter::queue(tmp);
    
    dbg1<<"Added controller \""<<name<<"\" with parameter "<<param<<endl;
    
    m_signal_controller_added(param);
    return ControllerIterator(new_vector->begin() + i);
#endif
    
    return curves_end();
  }
  
  
  void Pattern::remove_controller(ControllerIterator iter) {
    // find the element to erase
    unsigned i;
    for (i = 0; i < m_sd->curves->size(); ++i) {
      if ((*m_sd->curves)[i] == &*iter)
        break;
    }
    if (i >= m_sd->curves->size())
      return;
    
    // make a copy of the old vector
    vector<Curve*>* new_vector = new vector<Curve*>(*m_sd->curves);
    long param = (*m_sd->curves)[i]->get_param();
    new_vector->erase(new_vector->begin() + i);
    
    // delete the old vector
    vector<Curve*>* tmp = m_sd->curves;
    m_sd->curves = new_vector;
    Deleter::queue(tmp);
    
    dbg1<<"Removed controller"<<endl;

    m_signal_controller_removed(param);
  }
  */
  
  
  void Pattern::add_curve_point(CurveIterator iter, const SongTime& step, 
				int value) {
    
    // XXX This needs IMPLEMENTATION
    
    /*
    assert(step <= m_sd->length);
    (*iter.m_iterator)->add_point(step.get_beat(), value);
    //m_signal_cc_added((*iter.m_iterator)->get_info().get_number(), step, value);
    */
  }


  void Pattern::remove_curve_point(CurveIterator iter, const SongTime& step) {
    
    // XXX This needs IMPLEMENTATION
    
    /*
    assert(step < m_sd->length);
    (*iter.m_iterator)->remove_point(step.get_beat());
    //m_signal_cc_removed((*iter.m_iterator)->get_info().get_number(), step);
    */
  }

  
  unsigned int Pattern::get_steps() const {
    return 0;
  }


  const SongTime& Pattern::get_length() const {
    return m_length;
  }
  

  void Pattern::get_dirty_rect(int* minStep, int* minNote, 
                               int* maxStep, int* maxNote) const {
    if (minStep)
      *minStep = this->m_min_step;
    if (minNote)
      *minNote = this->m_min_note;
    if (maxStep)
      *maxStep = this->m_max_step;
    if (maxNote)
      *maxNote = this->m_max_note;
  }
  
  
  void Pattern::reset_dirty_rect() const {
    
    // XXX This needs IMPLEMENTATION
    
    /*
    m_min_step = m_sd->length.get_beat() * m_sd->steps;
    m_max_step = -1;
    m_min_note = 128;
    m_max_note = -1;
    */
  }

  
  bool Pattern::is_dirty() const {
    return m_dirty;
  }


  void Pattern::make_clean() const {
    m_dirty = false;
  }


  bool Pattern::fill_xml_node(Element* elt) const {
    char tmp_txt[256];
    elt->set_attribute("name", m_name);
    sprintf(tmp_txt, "%d", get_length().get_beat());
    elt->set_attribute("length", tmp_txt);
    sprintf(tmp_txt, "%d", get_steps());
    elt->set_attribute("steps", tmp_txt);

    NoteIterator iter;
    for (iter = notes_begin(); iter != notes_end(); ++iter) {
      Element* note_elt = elt->add_child("note");
      // XXX This needs IMPLEMENTATION
      /*
      sprintf(tmp_txt, "%d", iter->get_step());
      note_elt->set_attribute("step", tmp_txt);
      */
      sprintf(tmp_txt, "%d", iter->get_key());
      note_elt->set_attribute("value", tmp_txt);
      sprintf(tmp_txt, "%d", iter->get_length().get_beat());
      note_elt->set_attribute("length", tmp_txt);
      sprintf(tmp_txt, "%d", iter->get_velocity());
      note_elt->set_attribute("velocity", tmp_txt);
    }
    
    ConstCurveIterator citer;
    for (citer = curves_begin(); citer != curves_end(); ++citer) {
      Element* ctrl_elt = elt->add_child("controller");
      ctrl_elt->set_attribute("name", citer->get_info().get_name());
      sprintf(tmp_txt, "%lu", citer->get_info().get_number());
      ctrl_elt->set_attribute("param", tmp_txt);
      sprintf(tmp_txt, "%d", citer->get_info().get_min());
      ctrl_elt->set_attribute("min", tmp_txt);
      sprintf(tmp_txt, "%d", citer->get_info().get_max());
      ctrl_elt->set_attribute("max", tmp_txt);
      for (unsigned i = 0; i < citer->get_size(); ++i) {
        const InterpolatedEvent* e = citer->get_event(i);
        if (e && e->get_step() == i) {
          Element* pt_elt = ctrl_elt->add_child("point");
          sprintf(tmp_txt, "%lu", e->get_step());
          pt_elt->set_attribute("step", tmp_txt);
          sprintf(tmp_txt, "%d", e->get_start());
          pt_elt->set_attribute("value", tmp_txt);
        }
      }
      if (citer->get_event(citer->get_size() - 1)) {
        Element* pt_elt = ctrl_elt->add_child("point");
        sprintf(tmp_txt, "%lu", citer->get_size());
        pt_elt->set_attribute("step", tmp_txt);
        sprintf(tmp_txt, "%d", 
                citer->get_event(citer->get_size() - 1)->get_end());
        pt_elt->set_attribute("value", tmp_txt);
      }
    }
    
    return true;
  }


  bool Pattern::parse_xml_node(const Element* elt) {

    Node::NodeList nodes = elt->get_children("note");
    Node::NodeList::const_iterator iter;
    for (iter = nodes.begin(); iter != nodes.end(); ++iter) {
      const Element* note_elt = dynamic_cast<const Element*>(*iter);
      if (!note_elt)
        continue;
      int step, value, length, velocity;
      sscanf(note_elt->get_attribute("step")->get_value().c_str(), "%d", &step);
      sscanf(note_elt->get_attribute("value")->get_value().c_str(), 
             "%d", &value);
      sscanf(note_elt->get_attribute("length")->get_value().c_str(), 
             "%d", &length);
      velocity = 64;
      if (note_elt->get_attribute("velocity")) {
        sscanf(note_elt->get_attribute("velocity")->get_value().c_str(), 
               "%d", &velocity);
      }
      add_note(SongTime(step, 0), value, velocity, SongTime(length, 0));
    }
    
    // XXX Fix this!
    /*
    nodes = elt->get_children("controller");
    for (iter = nodes.begin(); iter != nodes.end(); ++iter) {
      const Element* ctrl_elt = dynamic_cast<const Element*>(*iter);
      if (!ctrl_elt)
        continue;
      unsigned long param;
      int min, max;
      string name;
      sscanf(ctrl_elt->get_attribute("param")->get_value().c_str(), 
             "%lu", &param);
      sscanf(ctrl_elt->get_attribute("min")->get_value().c_str(), "%d", &min);
      sscanf(ctrl_elt->get_attribute("max")->get_value().c_str(), "%d", &max);
      name = ctrl_elt->get_attribute("name")->get_value();
      CurveIterator citer = add_controller(name, param, min, max);
      Node::NodeList nodes2 = ctrl_elt->get_children("point");
      Node::NodeList::const_iterator iter2;
      for (iter2 = nodes2.begin(); iter2 != nodes2.end(); ++iter2) {
        const Element* pt_elt = dynamic_cast<const Element*>(*iter2);
        if (!pt_elt)
          continue;
        unsigned long step;
        int value;
        sscanf(pt_elt->get_attribute("step")->get_value().c_str(),
               "%lu", &step);
        sscanf(pt_elt->get_attribute("value")->get_value().c_str(),
               "%d", &value);
        add_cc(citer, step, value);
      }
    }
    */
    
    return true;
  }
  
  
  void Pattern::sequence(MIDIBuffer& buffer, const SongTime& from, 
			 const SongTime& to, 
                         const SongTime& pattern_length, int channel) const {
    /*
    unsigned long cc_steps = 0;
    
    // need to copy this because the editing thread might change it
    SeqData* sd = m_sd;
    
    from = from < 0 ? 0 : from;
    unsigned start = unsigned(floor(from * sd->steps));
    unsigned end = unsigned(ceil(to * sd->steps));
    end = (end > pattern_length * sd->steps ? pattern_length * sd->steps : end);
    double off_d = 0.001;
    double cc_pos = from;
    
    for (unsigned step = start; step < end; ++step) {
      
      // write note ons
      NoteEvent* event = (*sd->ons)[step];
      if (step / double(sd->steps) >= from) {
        while (event) {
          unsigned char* data = buffer.reserve(step / double(sd->steps), 3);
          if (data) {
            memcpy(data, event->get_data(), 3);
            data[0] |= (unsigned char)channel;
          }
          event = event->get_next();
        }
      }
      
      // write CCs
      for ( ; cc_pos < (step + 1) / double(sd->steps) && cc_pos < to; 
            cc_pos += buffer.get_cc_resolution()) {
        ++cc_steps;
        for (unsigned c = 0; c < sd->curves->size(); ++c)
	  (*sd->curves)[c]->write_events(buffer, cc_pos * sd->steps, 
					 cc_pos, channel);
      }
      
      // write note offs
      if (((step + 1) / double(sd->steps) - off_d < to) &&
          ((step + 1) / double(sd->steps) - off_d >= from)){
        event = (*sd->offs)[step];
        while (event) {
          unsigned char* data = 
            buffer.reserve((step + 1) / double(sd->steps) - off_d, 3);
          if (data) {
            memcpy(data, event->get_data(), 3);
            data[0] |= (unsigned char)channel;
          }
          event = event->get_next();
        }
        
        // write all notes off if this is the end of the sequence entry
        if (step == pattern_length * sd->steps - 1) {
          unsigned char all_notes_off[] = { 0xB0, 123, 0 };
          unsigned char* data = 
            buffer.reserve((step + 1) / double(sd->steps) - off_d, 3);
          if (data) {
            memcpy(data, all_notes_off, 3);
            data[0] |= (unsigned char)channel;
          }
        }
        
      }
      
    }
    */
  }
  

  Pattern::NoteIterator Pattern::find_note(const SongTime& time, 
					   int key) const {
    if (time >= get_length() || key >= 128)
      return NoteIterator(this, 0);
    
    NoteIterator iter = notes_begin();
    for ( ; iter != notes_end() && iter->get_time() <= time; ++iter) {
      if (iter->get_key() != key)
	continue;
      if (iter->get_time() <= time && 
	  iter->get_time() + iter->get_length() > time)
	return iter;
    }
    
    return notes_end();
  }

  
  const SongTime 
  Pattern::check_maximal_free_space(const SongTime& start, int key,
				    const SongTime& limit) const {
    if (key >= 128)
      return SongTime(0, 0);
    
    NoteIterator iter = notes_begin();
    for ( ; iter != notes_end() && iter->get_time() < start + limit; ++iter) {
      if (iter->get_key() != key)
	continue;
      if (iter->get_time() <= start)
	return iter->get_time() - start;
    }
    
    SongTime result = m_length - start;
    result = result > limit ? limit : result;
    
    return result;
  }
  

  bool Pattern::check_free_space(const SongTime& start, 
				 int key, const SongTime& length) const {
    NoteSelection empty(this);
    return check_free_space(start, key, length, empty);
  }


  bool Pattern::check_free_space(const SongTime& start, int key, 
				 const SongTime& length, 
				 const NoteSelection& ignore) const {
    if (key < 0 || key > 128 || length < SongTime(0, 0) || 
	start >= m_length || start + length > m_length)
      return false;
    
    // XXX Room for optimisation - we can do a log(N) search
    NoteIterator iter = notes_begin();
    for ( ; iter != notes_end(); ++iter) {
      if (iter->get_time() >= start + length)
	break;
      if (iter->get_key() == key && 
	  iter->get_time() < start + length &&
	  iter->get_time() + iter->get_length() > start &&
	  ignore.find(iter) == ignore.end())
	return false;
    }
    return true;
  }


  Pattern::CurveIterator Pattern::add_curve(const ControllerInfo& info) {
    
    // XXX This needs IMPLEMENTATION
    
    /*

    // make a copy of the curve vector and add the new curve
    vector<Curve*>* new_vector = new vector<Curve*>(*m_sd->curves);
    new_vector->push_back(new Curve(info, m_sd->steps * m_sd->length.get_beat()));
    
    // delete the old vector
    vector<Curve*>* tmp = m_sd->curves;
    m_sd->curves = new_vector;
    Deleter::queue(tmp);
    
    dbg1<<"Added curve \""<<info.get_name()<<"\" with parameter "
	<<info.get_number()<<endl;
    
    m_signal_curve_added(info.get_number());
    return CurveIterator(new_vector->begin() + (new_vector->size() - 1));
    */
    
    return curves_end();
  }


  Pattern::CurveIterator Pattern::add_curve(Curve* curve) {
    
    // XXX This needs IMPLEMENTATION
    
    /*
    
    // XXX need to check that there isn't a curve with this parameter number
    // already
    
    // check that the lengths match
    if (curve->get_size() != get_length().get_beat() * get_steps())
      return curves_end();
    
    // make a copy of the curve vector and add the new curve
    vector<Curve*>* new_vector = new vector<Curve*>(*m_sd->curves);
    new_vector->push_back(curve);
    
    // delete the old vector
    vector<Curve*>* tmp = m_sd->curves;
    m_sd->curves = new_vector;
    Deleter::queue(tmp);
    
    dbg1<<"Added curve \""<<curve->get_info().get_name()<<"\" with parameter "
	<<curve->get_info().get_number()<<endl;
    
    m_signal_curve_added(curve->get_info().get_number());
    return CurveIterator(new_vector->begin() + (new_vector->size() - 1));
    */
    
    return curves_end();
  }
  

  bool Pattern::remove_curve(Pattern::CurveIterator iter) {
    Curve* curve = disown_curve(iter);
    if (!curve)
      return false;
    Deleter::queue(curve);
    return curve;
  }


  Curve* Pattern::disown_curve(Pattern::CurveIterator iter) {
    
    // XXX This needs IMPLEMENTATION
    
    /*
   
    // find the element to erase
    unsigned i;
    for (i = 0; i < m_sd->curves->size(); ++i) {
      if ((*m_sd->curves)[i] == &*iter)
        break;
    }
    if (i >= m_sd->curves->size())
      return 0;
    
    // make a copy of the old vector
    Curve* curve = (*m_sd->curves)[i];
    vector<Curve*>* new_vector = new vector<Curve*>(*m_sd->curves);
    const ControllerInfo& ci = (*m_sd->curves)[i]->get_info();
    new_vector->erase(new_vector->begin() + i);
    
    // delete the old vector
    vector<Curve*>* tmp = m_sd->curves;
    m_sd->curves = new_vector;
    Deleter::queue(tmp);
    
    dbg1<<"Removed curve \""<<ci.get_name()<<"\" with parameter \""
	<<ci.get_number()<<"\""<<endl;

    m_signal_curve_removed(ci.get_number());    
    
    return curve;
    
    */
    
    return 0;
  }

  
  Pattern::ConstCurveIterator Pattern::curves_begin() const {
    // XXX This needs IMPLEMENTATION
    
    /*
    return ConstCurveIterator(m_sd->curves->begin());
    */
    
    return curves_end();
  }
  
  
  Pattern::ConstCurveIterator Pattern::curves_end() const {
    // XXX This needs IMPLEMENTATION
    
    /*
    return ConstCurveIterator(m_sd->curves->end());
    */
    return ConstCurveIterator();
  }
  
  
  Pattern::ConstCurveIterator Pattern::curves_find(long param) const {
    // XXX This needs IMPLEMENTATION
    
    /*

    for (unsigned i = 0; i < m_sd->curves->size(); ++i) {
      if ((*m_sd->curves)[i]->get_info().get_number() == param)
        return ConstCurveIterator(m_sd->curves->begin() + i);
    }
    */
    return curves_end();
  }

  
  Pattern::CurveIterator Pattern::curves_begin() {
    // XXX This needs IMPLEMENTATION
    
    /*
      return CurveIterator(m_sd->curves->begin());
    */
    return curves_end();
  }
  
  
  Pattern::CurveIterator Pattern::curves_end() {
    // XXX This needs IMPLEMENTATION
    
    /*
    return CurveIterator(m_sd->curves->end());
    */
    return CurveIterator();
  }
  
  
  Pattern::CurveIterator Pattern::curves_find(long param) {
    // XXX This needs IMPLEMENTATION
    
    /*
    for (unsigned i = 0; i < m_sd->curves->size(); ++i) {
      if ((*m_sd->curves)[i]->get_info().get_number() == param)
        return CurveIterator(m_sd->curves->begin() + i);
    }
    */
    return curves_end();
  }

  
  sigc::signal<void, string>& Pattern::signal_name_changed() const {
    return m_signal_name_changed;
  }


  sigc::signal<void, SongTime const&>& Pattern::signal_length_changed() const {
    return m_signal_length_changed;
  }


  sigc::signal<void, int>& Pattern::signal_steps_changed() const {
    return m_signal_steps_changed;
  }


  sigc::signal<void, Event const&>& Pattern::signal_note_added() const {
    return m_signal_note_added;
  }


  sigc::signal<void, Event const&>& Pattern::signal_note_changed() const {
    return m_signal_note_changed;
  }


  sigc::signal<void, Event const&>& Pattern::signal_note_removed() const {
    return m_signal_note_removed;
  }


  sigc::signal<void, int>& Pattern::signal_curve_added() const {
    return m_signal_curve_added;
  }


  sigc::signal<void, int>& Pattern::signal_curve_removed() const {
    return m_signal_curve_removed;
  }


}


