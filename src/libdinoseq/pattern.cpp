#include <sys/types.h>
#include <signal.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <set>

#include "debug.hpp"
#include "deleter.hpp"
#include "noteevent.hpp"
#include "pattern.hpp"


namespace Dino {
  
  
  Pattern::NoteIterator::NoteIterator() 
    : m_pattern(NULL),
      m_note(NULL) {

  }
  
  
  Pattern::NoteIterator::NoteIterator(const Pattern* pat, Note* note)
    : m_pattern(pat),
      m_note(note) {

  }
  
  
  const Note* Pattern::NoteIterator::operator*() const {
    return m_note;
  }
  
  
  const Note* Pattern::NoteIterator::operator->() const {
    return m_note;
  }
  
  
  bool Pattern::NoteIterator::operator==(const NoteIterator& iter) const {
    return (m_pattern == iter.m_pattern && m_note == iter.m_note);
  }
  
  
  bool Pattern::NoteIterator::operator!=(const NoteIterator& iter) const {
    return !operator==(iter);
  }
  
  
  Pattern::NoteIterator& Pattern::NoteIterator::operator++() {
    assert(m_pattern);
    assert(m_note);
    
    if (m_note->get_note_on()->get_next()) {
      m_note = static_cast<NoteEvent*>(m_note->get_note_on()->get_next())
	->get_note();
      return *this;
    }
    else {
      for (int i = m_note->get_step() + 1;
	   i < m_pattern->get_length() * m_pattern->get_steps(); ++i) {
	if (m_pattern->m_note_ons[i]) {
	  m_note = m_pattern->m_note_ons[i]->get_note();
	  return *this;
	}
      }
    }
    
    m_note = NULL;
    return *this;
  }

  
  Pattern::Pattern(const string& name, int length, int steps) 
    : m_name(name),
      m_length(length),
      m_steps(steps),
      m_dirty(false) {
    
    dbg1<<"Creating pattern \""<<m_name<<"\""<<endl;
    
    assert(m_steps > 0);
    assert(m_length > 0);
    
    m_min_step = m_length * m_steps;
    m_max_step = -1;
    m_min_note = 128;
    m_max_note = -1;
    
    for (unsigned i = 0; i < m_length * m_steps; ++i) {
      m_note_ons.push_back(NULL);
      m_note_offs.push_back(NULL);
    }
    
    // debugging
    Controller c(m_length * m_steps, 1);
    c.set_event(0, 64);
    c.set_event(3, 3);
    c.set_event(14, 34);
    c.set_event(16, 95);
    c.set_event(24, 101);
    m_controllers.push_back(c);
  }


  Pattern::~Pattern() {
    dbg1<<"Destroying pattern \""<<m_name<<"\""<<endl;
    NoteEventList::iterator iter;
    for (iter = m_note_ons.begin(); iter != m_note_ons.end(); ++iter) {
      NoteEvent* event = *iter;
      while (event) {
	NoteEvent* tmp = event;
	delete event;
	event = static_cast<NoteEvent*>(tmp->get_next());
      }
    }
    for (iter = m_note_offs.begin(); iter != m_note_offs.end(); ++iter) {
      NoteEvent* event = *iter;
      while (event) {
	NoteEvent* tmp = event;
	delete event;
	event = static_cast<NoteEvent*>(tmp->get_next());
      }
    }
  }


  const string& Pattern::get_name() const {
    return m_name;
  }


  Pattern::NoteIterator Pattern::notes_begin() const {
    for (unsigned i = 0; i < m_length * m_steps; ++i) {
      if (m_note_ons[i])
	return NoteIterator(this, m_note_ons[i]->get_note());
    }
    return notes_end();
  }

  
  Pattern::NoteIterator Pattern::notes_end() const {
    return NoteIterator(this, NULL);
  }


  const vector<Controller>& Pattern::get_controllers() const {
    return m_controllers;
  }


  void Pattern::set_name(const string& name) {
    if (name != m_name) {
      dbg1<<"Changing pattern name from \""<<m_name<<"\" to \""<<name<<"\""<<endl;
      m_name = name;
      signal_name_changed(m_name);
    }
  }


  /** This function tries to add a new note with the given parameters. If 
      there is another note starting at the same step and with the same value,
      the old note will be deleted. If there is a note that is playing at that
      step with the same value, the old note will be shortened so it ends just
      before @c step. 
  */
  void Pattern::add_note(unsigned step, int key, int velocity, 
			 int note_length) {
    assert(step < m_length * m_steps);
    assert(key < 128);
    assert(velocity < 128);
    assert(step + note_length <= m_length * m_steps);
    
    // check if the same key is already playing at this step, if so, 
    // shorten it (or delete it if it starts at this step)
    NoteIterator iter;
    if (iter = find_note(step, key)) {
      if (iter->get_step() == step)
	delete_note(iter);
      else
	resize_note(iter, step - iter->get_step());
    }
    
    // check if there is room for a note of the wanted length or if
    // it has to be shortened
    int new_length = note_length;
    if (iter = find_note_on(step + 1, step + note_length, key))
      new_length = iter->get_step() - step;
    
    // create the new objects
    NoteEvent* note_on = new NoteEvent(MIDIEvent::NoteOn, step, key, velocity);
    NoteEvent* note_off = 
      new NoteEvent(MIDIEvent::NoteOff, step + new_length - 1, key, 0);
    Note* note = new Note(note_on, note_off);
    note_on->set_note(note);
    note_off->set_note(note);
    
    // add the new events
    // this must be done in a safe way since the sequencer could be sequencing
    // this pattern right now!
    // let's hope that pointer assignments are atomic...
    NoteEvent* old_note_off = m_note_offs[step + new_length - 1];
    note_off->set_next(old_note_off);
    if (old_note_off)
      old_note_off->set_previous(note_off);
    m_note_offs[step + new_length - 1] = note_off;
    NoteEvent* old_note_on = m_note_ons[step];
    note_on->set_next(old_note_on);
    if (old_note_on)
      old_note_on->set_previous(note_on);
    m_note_ons[step] = note_on;
    
    signal_note_added(step, key, new_length);
  }


  /** This function will delete the note with the given value playing at the
      given step, if there is one. It will also update the @c previous and 
      @c next pointers so the doubly linked list will stay consistent with the
      note map. It will return the step that the deleted note started on,
      or -1 if no note was deleted. */
  void Pattern::delete_note(NoteIterator iterator) {
    assert(iterator);
    assert(iterator.m_pattern == this);
    
    Note* note = iterator.m_note;
    NoteEvent* previous;
    NoteEvent* next;
    
    // remove the note off event first
    // must be threadsafe!
    previous = note->m_note_off->get_previous();
    next = static_cast<NoteEvent*>(note->m_note_off->get_next());
    if (next)
      next->set_previous(previous);
    if (previous)
      previous->set_next(next);
    else
      m_note_offs[note->m_note_off->get_step()] = next;
    
    // then the note on event
    // must be threadsafe!
    previous = note->m_note_on->get_previous();
    next = static_cast<NoteEvent*>(note->m_note_on->get_next());
    if (next)
      next->set_previous(previous);
    if (previous)
      previous->set_next(next);
    else
      m_note_ons[note->m_note_on->get_step()] = next;
    
    // delete the note object and queue the events for deletion
    signal_note_removed(note->get_step(), note->get_key());
    note->m_note_on->queue_deletion();
    note->m_note_off->queue_deletion();
    delete note;
    
  }


  int Pattern::resize_note(NoteIterator iterator, int length) {
    assert(iterator);
    assert(iterator.m_pattern == this);
    assert(iterator->get_step() + length <= m_length * m_steps);
    
    // check if there is room for a note of the wanted length or if
    // it has to be shortened
    int new_length = length;
    NoteIterator iter;
    if (iter = find_note_on(iterator->get_step() + 1, 
			    iterator->get_step() + length, 
			    iterator->get_key()))
      new_length = iter->get_step() - iterator->get_step();
    
    // remove the note off event
    // must be threadsafe!
    Note* note = iterator.m_note;
    NoteEvent* previous = note->m_note_off->get_previous();
    NoteEvent* next = static_cast<NoteEvent*>(note->m_note_off->get_next());
    if (next)
      next->set_previous(previous);
    if (previous)
      previous->set_next(next);
    else
      m_note_offs[note->m_note_off->get_step()] = next;
    
    // insert the note off event at the new position
    // must be threadsafe!
    note->m_note_off->set_step(note->get_step() + new_length - 1);
    NoteEvent* old_note_off = m_note_offs[note->get_step() + new_length - 1];
    note->m_note_off->set_next(old_note_off);
    if (old_note_off)
      old_note_off->set_previous(note->m_note_off);
    m_note_offs[iterator->get_step() + new_length - 1] = note->m_note_off;
    
    signal_note_changed(iterator->get_step(), iterator->get_key(),
			iterator->get_length());
    
    return iterator->get_length();
  }


  void Pattern::set_velocity(NoteIterator iterator, unsigned char velocity) {
    assert(iterator);
    assert(iterator.m_pattern == this);
    assert(velocity < 128);
    
    iterator.m_note->m_note_on->set_velocity(velocity);
    
    signal_note_changed(iterator->get_step(), iterator->get_key(),
			iterator->get_length());
  }


  void Pattern::add_cc(unsigned int controller, unsigned int step, 
		       unsigned char value) {
    
  }


  void Pattern::remove_cc(unsigned int controller, unsigned int step) {
    
  }

  
  int Pattern::get_steps() const {
    return m_steps;
  }


  int Pattern::get_length() const {
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
  
  
  void Pattern::reset_dirty_rect() {
    m_min_step = m_length * m_steps;
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
    char tmp_txt[10];
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
    
    return true;
  }


  bool Pattern::parse_xml_node(const Element* elt) {
    m_name = "";
    Node::NodeList nodes = elt->get_children("name");
    if (nodes.begin() != nodes.end()) {
      const Element* name_elt = dynamic_cast<const Element*>(*nodes.begin());
      if (name_elt) {
	m_name = name_elt->get_child_text()->get_content();
	signal_name_changed(m_name);
      }
    }

    nodes = elt->get_children("note");
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
    return true;
  }


  int Pattern::get_events(double beat, double before_beat,
			  const MIDIEvent** events, double* beats, 
			  int room) const {
    assert(beat < m_length);
    assert(before_beat <= m_length);
    
    int list_no = 0;
    /* XXX This needs:
     *      range checking for the events array
     *      note off events!
     */
    double off_d = 0.001;
    unsigned step;
    for (step = unsigned(ceil(beat * m_steps));
	 step < before_beat * m_steps; ++step) {
      
      // note off events just before this step
      if (step > 0 && m_note_offs[step-1] && m_steps * (beat + off_d) <= step) {
	events[list_no] = m_note_offs[step - 1];
	beats[list_no] = step / double(m_steps) - off_d;
	++list_no;
      }
      
      // note on events on this step
      if (m_note_ons[step]) {
	events[list_no] = m_note_ons[step];
	beats[list_no] = step / double(m_steps);
	++list_no;
      }
      
      // controllers
      for (unsigned i = 0; i < m_controllers.size(); ++i) {
	if (m_controllers[i].get_event(step)) {
	  events[list_no] = m_controllers[i].get_event(step);
	  beats[list_no] = step / double(m_steps);
	  ++list_no;
	}
      }
      
    }
    
    // note off events after the last step
    if (m_steps * (before_beat + off_d) > step && m_note_offs[step - 1]) {
      events[list_no] = m_note_offs[step - 1];
      beats[list_no] = step / double(m_steps) - off_d;
      ++list_no;
    }
    
    return list_no;
  }
  

  Pattern::NoteIterator Pattern::find_note(unsigned step, int value) const {
    assert(step < m_length * m_steps);
    assert(value < 128);
    
    // iterate backwards until we find a note on event
    for (int i = step; i >= 0; --i) {
      NoteEvent* note_on = m_note_ons[i];
      
      // look for a note on event with the right key
      while (note_on) {
	
	if (note_on->get_key() == value) {
	  if (note_on->get_note()->m_note_off->get_step() >= step)
	    return NoteIterator(this, note_on->get_note());
	  else
	    return NoteIterator(this, NULL);
	}
	note_on = static_cast<NoteEvent*>(note_on->get_next());
      }
      
    }
    
    return NoteIterator(this, NULL);
  }

  
  Pattern::NoteIterator Pattern::find_note_on(unsigned start, unsigned end, 
					      unsigned char key) {
    for (unsigned i = start; i < end; ++i) {
      NoteEvent* note_on = m_note_ons[i];
      while (note_on) {
	if (note_on->get_key() == key)
	  return NoteIterator(this, note_on->get_note());
	note_on = static_cast<NoteEvent*>(note_on->get_next());
      }
    }
    
    return NoteIterator(this, NULL);
  }
  

}


