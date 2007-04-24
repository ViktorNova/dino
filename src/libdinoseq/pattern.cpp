/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
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
#include "patternselection.hpp"


using namespace std;


namespace Dino {
  
  
  Pattern::NoteIterator::NoteIterator() 
    : m_pattern(0),
      m_note(0) {

  }
  
  
  Pattern::NoteIterator::NoteIterator(const Pattern* pat, Note* note)
    : m_pattern(pat),
      m_note(note) {

  }
  
  
  Note& Pattern::NoteIterator::operator*() const {
    return *m_note;
  }
  
  
  Note* Pattern::NoteIterator::operator->() const {
    return m_note;
  }
  
  
  bool Pattern::NoteIterator::operator==(const NoteIterator& iter) const {
    return (m_pattern == iter.m_pattern && m_note == iter.m_note);
  }
  
  
  bool Pattern::NoteIterator::operator!=(const NoteIterator& iter) const {
    return !operator==(iter);
  }
  
  
  bool Pattern::NoteIterator::operator<(const NoteIterator& iter) const {
    return (m_note->get_step() < iter->get_step()) || 
      ((m_note->get_step() == iter->get_step()) && 
       (m_note->get_key() < iter->get_key()));
  }


  Pattern::NoteIterator& Pattern::NoteIterator::operator++() {
    assert(m_pattern);
    assert(m_note);
    
    if (m_note->get_note_on()->get_next()) {
      m_note = m_note->get_note_on()->get_next()->get_note();
      return *this;
    }
    else {
      for (unsigned int i = m_note->get_step() + 1;
           i < m_pattern->get_length() * m_pattern->get_steps(); ++i) {
        if ((*m_pattern->m_sd->ons)[i]) {
          m_note = (*m_pattern->m_sd->ons)[i]->get_note();
          return *this;
        }
      }
    }
    
    m_note = 0;
    return *this;
  }


  Pattern::NoteIterator Pattern::NoteIterator::operator++(int) {
    NoteIterator result = *this;
    ++(*this);
    return result;
  }

  
  Pattern::Pattern(int id, const string& name, int length, int steps) 
    : m_id(id),
      m_name(name),
      m_sd(new SeqData(new NoteEventList(length * steps),
                       new NoteEventList(length * steps),
                       new vector<Curve*>(), length, steps)),
      m_dirty(false) {
    
    dbg1<<"Creating pattern \""<<m_name<<"\""<<endl;
    
    assert(m_sd->steps > 0);
    assert(m_sd->length > 0);
    
    m_min_step = m_sd->length * m_sd->steps;
    m_max_step = -1;
    m_min_note = 128;
    m_max_note = -1;
  }


  Pattern::Pattern(int id, const Pattern& pat) 
    : m_id(id),
      m_name(pat.get_name()),
      m_sd(new SeqData(new NoteEventList(pat.get_length() * pat.get_steps()),
                       new NoteEventList(pat.get_length() * pat.get_steps()),
                       new vector<Curve*>(), 
                       pat.get_length(), pat.get_steps())),
      m_dirty(false) {

    dbg1<<"Duplicating pattern \""<<pat.get_name()<<"\""<<endl;
    
    assert(m_sd->steps > 0);
    assert(m_sd->length > 0);
    
    m_min_step = m_sd->length * m_sd->steps;
    m_max_step = -1;
    m_min_note = 128;
    m_max_note = -1;
    
    // copy all notes
    NoteIterator iter;
    for (iter = pat.notes_begin(); iter != pat.notes_end(); ++iter) {
      add_note(iter->get_step(), iter->get_key(), 
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
    for (iter = m_sd->ons->begin(); iter != m_sd->ons->end(); ++iter) {
      NoteEvent* event = *iter;
      while (event) {
        NoteEvent* tmp = event;
        event = tmp->get_next();
        delete tmp;
      }
    }
    
    // delete all note off events
    for (iter = m_sd->offs->begin(); iter != m_sd->offs->end(); ++iter) {
      NoteEvent* event = *iter;
      while (event) {
        NoteEvent* tmp = event;
        event = tmp->get_next();
        delete tmp;
      }
    }
    
    // delete all containers
    delete m_sd;
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
    for (unsigned i = 0; i < m_sd->length * m_sd->steps; ++i) {
      if ((*m_sd->ons)[i])
        return NoteIterator(this, (*m_sd->ons)[i]->get_note());
    }
    return notes_end();
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


  void Pattern::set_length(unsigned int length) {
    assert(length > 0);
    
    // no change
    if (length == m_sd->length)
      return;
    
    // the new length is shorter, we may have to delete and resize notes
    if (length < m_sd->length) {
      for (unsigned i = length * m_sd->steps; 
           i < m_sd->length * m_sd->steps; ++i) {
        while ((*m_sd->ons)[i] != 0)
          delete_note((*m_sd->ons)[i]->get_note());
      }
      for (unsigned i = length * m_sd->steps; 
           i < m_sd->length * m_sd->steps; ++i) {
        while ((*m_sd->offs)[i] != 0) {
          Note* note = (*m_sd->offs)[i]->get_note();
          resize_note(note, length * m_sd->steps - note->get_step());
        }
      }
    }
    
    NoteEventList* new_note_ons = new NoteEventList(*m_sd->ons);
    NoteEventList* new_note_offs = new NoteEventList(*m_sd->offs);
    vector<Curve*>* new_controllers = new vector<Curve*>();
    
    new_note_ons->resize(length * m_sd->steps);
    new_note_offs->resize(length * m_sd->steps);
    
    // iterate over all controllers
    for (unsigned i = 0; i < m_sd->curves->size(); ++i) {
      Curve* c = (*m_sd->curves)[i];
      
      // create a new controller with the same settings but different size
      Curve* new_c = new Curve(c->get_info(), length * m_sd->steps);
      new_controllers->push_back(new_c);
      
      // copy all the controller events that fit into the new size
      for (unsigned j = 0; j < length * m_sd->steps && 
             j < m_sd->length * m_sd->steps; ++j) {
        const InterpolatedEvent* cce = c->get_event(j);
        if (cce)
          new_c->add_point(j, cce->get_start());
      }
    }
    
    // swap the SeqData pointer
    SeqData* old_sd = m_sd;
    m_sd = new SeqData(new_note_ons, new_note_offs, new_controllers,
                       length, old_sd->steps);
    Deleter::queue(old_sd);

    m_signal_length_changed(m_sd->length);
  }


  void Pattern::set_steps(unsigned int steps) {
    assert(steps > 0);
    
    if (steps == m_sd->steps)
      return;
    
    // allocate new data structures
    NoteEventList* new_note_ons = new NoteEventList(steps * m_sd->length);
    NoteEventList* new_note_offs = new NoteEventList(steps * m_sd->length);
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
      Curve* new_c = new Curve(c->get_info(), m_sd->length * steps);
      new_controllers->push_back(new_c);
      
      // copy the events from the old controller to the new controller
      // this may not be a 1 to 1 copy since the step resolutions differ
      for (unsigned j = 0; j < m_sd->length * m_sd->steps; ++j) {
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
      add_note(starts[i], keys[i], velocities[i], lengths[i]);
    
    m_signal_steps_changed(m_sd->length);
  }


  /** This function tries to add a new note with the given parameters. If 
      there is another note starting at the same step and with the same value,
      the old note will be deleted. If there is a note that is playing at that
      step with the same value, the old note will be shortened so it ends just
      before @c step. 
  */
  Pattern::NoteIterator Pattern::add_note(unsigned step, int key, int velocity, 
                                          int note_length) {
    assert(step < m_sd->length * m_sd->steps);
    assert(key < 128);
    assert(velocity < 128);
    assert(note_length > 0);
    
    // check if the same key is already playing at this step, if so, 
    // shorten it (or delete it if it starts at this step)
    NoteIterator iter;
    if ((iter = find_note(step, key)) != notes_end()) {
      if (iter->get_step() == step)
        delete_note(iter);
      else
        resize_note(iter, step - iter->get_step());
    }
    
    // check if there is room for a note of the wanted length or if
    // it has to be shortened
    int new_length = note_length;
    if (step + new_length > m_sd->length * m_sd->steps)
      new_length = m_sd->length * m_sd->steps - step;
    if ((iter = find_note_on(step + 1, step + new_length, key)) != notes_end())
      new_length = iter->get_step() - step;


    
    // create the new objects
    NoteEvent* note_on = new NoteEvent(NoteEvent::NoteOn, step, key, velocity);
    NoteEvent* note_off = 
      new NoteEvent(NoteEvent::NoteOff, step + new_length - 1, key, 0);
    Note* note = new Note(note_on, note_off);
    note_on->set_note(note);
    note_off->set_note(note);
    
    // add the new events
    // this must be done in a safe way since the sequencer could be sequencing
    // this pattern right now!
    // let's hope that pointer assignments are atomic...
    NoteEvent* old_note_off = (*m_sd->offs)[step + new_length - 1];
    note_off->set_next(old_note_off);
    if (old_note_off)
      old_note_off->set_previous(note_off);
    (*m_sd->offs)[step + new_length - 1] = note_off;
    NoteEvent* old_note_on = (*m_sd->ons)[step];
    note_on->set_next(old_note_on);
    if (old_note_on)
      old_note_on->set_previous(note_on);
    (*m_sd->ons)[step] = note_on;
    
    m_signal_note_added(*note);
    
    return NoteIterator(this, note);
  }


  void Pattern::add_notes(const NoteCollection& notes, unsigned step, int key,
                          PatternSelection* selection) {
    assert(step < m_sd->length * m_sd->steps);
    assert(key < 128);
    
    if (selection)
      selection->clear();
    
    NoteCollection::ConstIterator iter;
    for (iter = notes.begin(); iter != notes.end(); ++iter) {
      if (iter->start + step >= m_sd->length * m_sd->steps)
        continue;
      if (iter->key + key < 128)
        continue;
      NoteIterator iter2 = add_note(iter->start + step, iter->key + key - 127, 
                                    iter->velocity, iter->length);
      if (selection)
        selection->add_note(iter2);
    }
  }


  /** This function will delete the note with the given value playing at the
      given step, if there is one. It will also update the @c previous and 
      @c next pointers so the doubly linked list will stay consistent with the
      note map. It will return the step that the deleted note started on,
      or -1 if no note was deleted. */
  void Pattern::delete_note(NoteIterator iterator) {
    assert(iterator != notes_end());
    assert(iterator.m_pattern == this);

    delete_note(iterator.m_note);
  }


  void Pattern::delete_note(Note* note) {
    NoteEvent* previous;
    NoteEvent* next;
    
    // remove the note off event first
    // must be threadsafe!
    previous = note->m_note_off->get_previous();
    next = note->m_note_off->get_next();
    if (next)
      next->set_previous(previous);
    if (previous)
      previous->set_next(next);
    else
      (*m_sd->offs)[note->m_note_off->get_step()] = next;
    
    // then the note on event
    // must be threadsafe!
    previous = note->m_note_on->get_previous();
    next = note->m_note_on->get_next();
    if (next)
      next->set_previous(previous);
    if (previous)
      previous->set_next(next);
    else
      (*m_sd->ons)[note->m_note_on->get_step()] = next;
    
    // delete the note object and queue the events for deletion
    m_signal_note_removed(*note);
    Deleter::queue(note->m_note_on);
    Deleter::queue(note->m_note_off);
    Deleter::queue(note);
  }


  int Pattern::resize_note(NoteIterator iterator, int length) {
    assert(iterator != notes_end());
    assert(iterator.m_pattern == this);
    
    if (iterator->get_step() + length > m_sd->length * m_sd->steps)
      length = m_sd->length * m_sd->steps - iterator->get_step();
    
    return resize_note(iterator.m_note, length);
  }


  int Pattern::resize_note(Note* note, int length) {
    
    // check if there is room for a note of the wanted length or if
    // it has to be shortened
    int new_length = length;
    NoteIterator iter;
    if ((iter = find_note_on(note->get_step() + 1, 
                             note->get_step() + length, 
                             note->get_key())) != notes_end())
      new_length = iter->get_step() - note->get_step();
    
    // remove the note off event
    // must be threadsafe!
    NoteEvent* previous = note->m_note_off->get_previous();
    NoteEvent* next = note->m_note_off->get_next();
    if (next)
      next->set_previous(previous);
    if (previous)
      previous->set_next(next);
    else
      (*m_sd->offs)[note->m_note_off->get_step()] = next;
    
    // insert the note off event at the new position
    // must be threadsafe!
    note->m_note_off->set_step(note->get_step() + new_length - 1);
    NoteEvent* old_note_off = (*m_sd->offs)[note->get_step() + new_length - 1];
    note->m_note_off->set_next(old_note_off);
    if (old_note_off)
      old_note_off->set_previous(note->m_note_off);
    (*m_sd->offs)[note->get_step() + new_length - 1] = note->m_note_off;
    
    m_signal_note_changed(*note);
    
    return note->get_length();
  }


  void Pattern::set_velocity(NoteIterator iterator, unsigned char velocity) {
    assert(iterator != notes_end());
    assert(iterator.m_pattern == this);
    assert(velocity < 128);
    
    iterator.m_note->m_note_on->set_velocity(velocity);
    
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
  
  
  void Pattern::add_curve_point(CurveIterator iter, unsigned int step, 
				int value) {
    assert(step <= m_sd->length * m_sd->steps);
    (*iter.m_iterator)->add_point(step, value);
    m_signal_cc_added((*iter.m_iterator)->get_info().get_number(), step, value);
  }


  void Pattern::remove_curve_point(CurveIterator iter, unsigned int step) {
    assert(step < m_sd->length * m_sd->steps);
    (*iter.m_iterator)->remove_point(step);
    m_signal_cc_removed((*iter.m_iterator)->get_info().get_number(), step);
  }

  
  unsigned int Pattern::get_steps() const {
    return m_sd->steps;
  }


  unsigned int Pattern::get_length() const {
    return m_sd->length;
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
  
  
  void Pattern::reset_dirty_rect() {
    m_min_step = m_sd->length * m_sd->steps;
    m_max_step = -1;
    m_min_note = 128;
    m_max_note = -1;
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
    sprintf(tmp_txt, "%d", get_length());
    elt->set_attribute("length", tmp_txt);
    sprintf(tmp_txt, "%d", get_steps());
    elt->set_attribute("steps", tmp_txt);

    NoteIterator iter;
    for (iter = notes_begin(); iter != notes_end(); ++iter) {
      Element* note_elt = elt->add_child("note");
      sprintf(tmp_txt, "%d", iter->get_step());
      note_elt->set_attribute("step", tmp_txt);
      sprintf(tmp_txt, "%d", iter->get_key());
      note_elt->set_attribute("value", tmp_txt);
      sprintf(tmp_txt, "%d", iter->get_length());
      note_elt->set_attribute("length", tmp_txt);
      sprintf(tmp_txt, "%d", iter->get_velocity());
      note_elt->set_attribute("velocity", tmp_txt);
    }
    
    CurveIterator citer;
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
      add_note(step, value, velocity, length);
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
  
  
  void Pattern::sequence(MIDIBuffer& buffer, double from, double to, 
                         unsigned int pattern_length, int channel) const {
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
        for (unsigned c = 0; c < sd->curves->size(); ++c) {
          const InterpolatedEvent* event = (*sd->curves)[c]->get_event(step);
          if (event) {
            unsigned char* data = buffer.
              reserve(cc_pos, 3);
            if (data && is_cc((*sd->curves)[c]->get_info().get_number())) {
              data[0] = 0xB0 | (unsigned char)channel;
              data[1] = cc_number((*sd->curves)[c]->get_info().get_number());
              data[2] = (unsigned char)
                (event->get_start() + (cc_pos * sd->steps - event->get_step()) *
                 ((event->get_end() - event->get_start()) /
                  double(event->get_length())));
            }
            else if (data && is_pbend((*sd->curves)[c]->
				      get_info().get_number())) {
              data[0] = 0xE0 | (unsigned char)channel;
              int value = int(event->get_start() + 
                              (cc_pos * sd->steps - event->get_step()) *
                              ((event->get_end() - event->get_start()) /
                               double(event->get_length())));
              data[1] = (value + 8192) & 0x7F;
              data[2] = ((value + 8192) >> 7) & 0x7F;
            }
          }
        }
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
    
  }
  

  Pattern::NoteIterator Pattern::find_note(unsigned step, int value) const {
    assert(step < m_sd->length * m_sd->steps);
    assert(value < 128);
    
    // iterate backwards until we find a note on event
    for (int i = step; i >= 0; --i) {
      NoteEvent* note_on = (*m_sd->ons)[i];
      
      // look for a note on event with the right key
      while (note_on) {
  
        if (note_on->get_key() == value) {
          if (note_on->get_note()->m_note_off->get_step() >= step)
            return NoteIterator(this, note_on->get_note());
          else
            return NoteIterator(this, 0);
        }
        note_on = note_on->get_next();
      }
      
    }
    
    return NoteIterator(this, 0);
  }


  Pattern::CurveIterator Pattern::add_curve(const ControllerInfo& info) {
    return CurveIterator(m_sd->curves->end());
  }
  

  bool Pattern::remove_curve(Pattern::CurveIterator iter) {
    return false;
  }

  
  Pattern::CurveIterator Pattern::curves_begin() const {
    return CurveIterator(m_sd->curves->begin());
  }
  
  
  Pattern::CurveIterator Pattern::curves_end() const {
    return CurveIterator(m_sd->curves->end());
  }
  
  
  Pattern::CurveIterator Pattern::curves_find(long param) const {
    for (unsigned i = 0; i < m_sd->curves->size(); ++i) {
      if ((*m_sd->curves)[i]->get_info().get_number() == param)
        return CurveIterator(m_sd->curves->begin() + i);
    }
    return curves_end();
  }

  
  Pattern::NoteIterator Pattern::find_note_on(unsigned start, unsigned end, 
                                              unsigned char key) {
    for (unsigned i = start; i < end; ++i) {
      NoteEvent* note_on = (*m_sd->ons)[i];
      while (note_on) {
        if (note_on->get_key() == key)
          return NoteIterator(this, note_on->get_note());
        note_on = note_on->get_next();
      }
    }
    
    return NoteIterator(this, 0);
  }


  Pattern::SeqData::SeqData(NoteEventList* note_ons, NoteEventList* note_offs, 
                            std::vector<Curve*>* controllers,
                            unsigned int l, unsigned int s)
    : ons(note_ons), offs(note_offs), curves(controllers),
      length(l), steps(s) {
    assert(ons != 0);
    assert(offs != 0);
    assert(curves != 0);
    assert(length > 0);
    assert(steps > 0);
  }

  
  Pattern::SeqData::~SeqData() {
    delete ons;
    delete offs;
    for (unsigned i = 0; i < curves->size(); ++i)
      delete (*curves)[i];
    delete curves;
  }


  sigc::signal<void, string>& Pattern::signal_name_changed() {
    return m_signal_name_changed;
  }


  sigc::signal<void, int>& Pattern::signal_length_changed() {
    return m_signal_length_changed;
  }


  sigc::signal<void, int>& Pattern::signal_steps_changed() {
    return m_signal_steps_changed;
  }


  sigc::signal<void, Note const&>& Pattern::signal_note_added() {
    return m_signal_note_added;
  }


  sigc::signal<void, Note const&>& Pattern::signal_note_changed() {
    return m_signal_note_changed;
  }


  sigc::signal<void, Note const&>& Pattern::signal_note_removed() {
    return m_signal_note_removed;
  }


  sigc::signal<void, int, int, int>& Pattern::signal_cc_added() {
    return m_signal_cc_added;
  }


  sigc::signal<void, int, int, int>& Pattern::signal_cc_changed() {
    return m_signal_cc_changed;
  }


  sigc::signal<void, int, int>& Pattern::signal_cc_removed() {
    return m_signal_cc_removed;
  }


  sigc::signal<void, int>& Pattern::signal_controller_added() {
    return m_signal_controller_added;
  }


  sigc::signal<void, int>& Pattern::signal_controller_removed() {
    return m_signal_controller_removed;
  }


}


