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


using namespace std;


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
	if ((*m_pattern->m_sd->ons)[i]) {
	  m_note = (*m_pattern->m_sd->ons)[i]->get_note();
	  return *this;
	}
      }
    }
    
    m_note = NULL;
    return *this;
  }

  
  Pattern::Pattern(int id, const string& name, int length, int steps) 
    : m_id(id),
      m_name(name),
      //m_length(length),
      //m_steps(steps),
      //m_note_ons(new NoteEventList(length * steps)),
      //m_note_offs(new NoteEventList(length * steps)),
      //m_controllers(new vector<Controller*>()),
      m_sd(new SeqData(new NoteEventList(length * steps),
		       new NoteEventList(length * steps),
		       new vector<Controller*>(), length, steps)),
      m_dirty(false) {
    
    dbg1<<"Creating pattern \""<<m_name<<"\""<<endl;
    
    assert(m_sd->steps > 0);
    assert(m_sd->length > 0);
    
    m_min_step = m_sd->length * m_sd->steps;
    m_max_step = -1;
    m_min_note = 128;
    m_max_note = -1;
  }


  Pattern::~Pattern() {
    dbg1<<"Destroying pattern \""<<m_name<<"\""<<endl;
    NoteEventList::iterator iter;
    
    // delete all note on events
    for (iter = m_sd->ons->begin(); iter != m_sd->ons->end(); ++iter) {
      NoteEvent* event = *iter;
      while (event) {
	NoteEvent* tmp = event;
	delete event;
	event = static_cast<NoteEvent*>(tmp->get_next());
      }
    }
    
    // delete all note off events
    for (iter = m_sd->offs->begin(); iter != m_sd->offs->end(); ++iter) {
      NoteEvent* event = *iter;
      while (event) {
	NoteEvent* tmp = event;
	delete event;
	event = static_cast<NoteEvent*>(tmp->get_next());
      }
    }
    
    // delete the controllers
    for (unsigned i = 0; i < m_sd->ctrls->size(); ++i)
      delete (*m_sd->ctrls)[i];
    
    // delete all containers
    delete m_sd;
  }
  
  
  int Pattern::get_id() const {
    return m_id;
  }
  

  const string& Pattern::get_name() const {
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
    return NoteIterator(this, NULL);
  }

  
  void Pattern::set_name(const string& name) {
    if (name != m_name) {
      dbg1<<"Changing pattern name from \""<<m_name<<"\" to \""
	  <<name<<"\""<<endl;
      m_name = name;
      signal_name_changed(m_name);
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
    vector<Controller*>* new_controllers = new vector<Controller*>();
    
    new_note_ons->resize(length * m_sd->steps);
    new_note_offs->resize(length * m_sd->steps);
    
    // iterate over all controllers
    for (unsigned i = 0; i < m_sd->ctrls->size(); ++i) {
      const Controller* c = (*m_sd->ctrls)[i];
      
      // create a new controller with the same settings but different size
      Controller* new_c = new Controller(c->get_name(), length * m_sd->steps,
					 c->get_param(), 
					 c->get_min(), c->get_max());
      new_controllers->push_back(new_c);
      
      // copy all the controller events that fit into the new size
      for (unsigned j = 0; j < length * m_sd->steps && 
	     j < m_sd->length * m_sd->steps; ++j) {
	const CCEvent* cce = c->get_event(j);
	if (cce)
	  new_c->set_event(j, cce->get_value());
      }
    }
    
    // swap the SeqData pointer
    SeqData* old_sd = m_sd;
    m_sd = new SeqData(new_note_ons, new_note_offs, new_controllers,
		       length, old_sd->steps);
    Deleter::queue(old_sd);

    signal_length_changed(m_sd->length);
  }


  void Pattern::set_steps(unsigned int steps) {
    assert(steps > 0);
    // XXX Copy note events to the new note event lists!
    
    if (steps == m_sd->steps)
      return;
    
    NoteEventList* new_note_ons = new NoteEventList(steps * m_sd->length);
    NoteEventList* new_note_offs = new NoteEventList(steps * m_sd->length);
    vector<Controller*>* new_controllers = new vector<Controller*>();
    
    // iterate over all controllers in the old controller list
    for (unsigned i = 0; i < m_sd->ctrls->size(); ++i) {
      const Controller* c = (*m_sd->ctrls)[i];
      
      // create a new controller with the same settings but different size
      Controller* new_c = new Controller(c->get_name(), m_sd->length * steps,
					 c->get_param(), 
					 c->get_min(), c->get_max());
      new_controllers->push_back(new_c);
      
      // copy the events from the old controller to the new controller
      // this may not be a 1 to 1 copy since the step resolutions differ
      for (unsigned j = 0; j < m_sd->length * m_sd->steps; ++j) {
	const CCEvent* e = c->get_event(j);
	if (e != 0) {
	  new_c->set_event(unsigned(steps * j / double(m_sd->steps)), 
			   e->get_value());
	}
      }
    }
    
    // swap the SeqData pointer
    SeqData* old_sd = m_sd;
    m_sd = new SeqData(new_note_ons, new_note_offs, new_controllers, 
		       old_sd->length, steps);
    Deleter::queue(old_sd);
    
    signal_steps_changed(m_sd->length);
  }


  /** This function tries to add a new note with the given parameters. If 
      there is another note starting at the same step and with the same value,
      the old note will be deleted. If there is a note that is playing at that
      step with the same value, the old note will be shortened so it ends just
      before @c step. 
  */
  void Pattern::add_note(unsigned step, int key, int velocity, 
			 int note_length) {
    assert(step < m_sd->length * m_sd->steps);
    assert(key < 128);
    assert(velocity < 128);
    assert(step + note_length <= m_sd->length * m_sd->steps);
    
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
    
    delete_note(iterator.m_note);
  }


  void Pattern::delete_note(Note* note) {
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
      (*m_sd->offs)[note->m_note_off->get_step()] = next;
    
    // then the note on event
    // must be threadsafe!
    previous = note->m_note_on->get_previous();
    next = static_cast<NoteEvent*>(note->m_note_on->get_next());
    if (next)
      next->set_previous(previous);
    if (previous)
      previous->set_next(next);
    else
      (*m_sd->ons)[note->m_note_on->get_step()] = next;
    
    // delete the note object and queue the events for deletion
    signal_note_removed(note->get_step(), note->get_key());
    Deleter::queue(note->m_note_on);
    Deleter::queue(note->m_note_off);
    Deleter::queue(note);
  }


  int Pattern::resize_note(NoteIterator iterator, int length) {
    assert(iterator);
    assert(iterator.m_pattern == this);
    assert(iterator->get_step() + length <= m_sd->length * m_sd->steps);
    
    return resize_note(iterator.m_note, length);
  }


  int Pattern::resize_note(Note* note, int length) {
    
    // check if there is room for a note of the wanted length or if
    // it has to be shortened
    int new_length = length;
    NoteIterator iter;
    if (iter = find_note_on(note->get_step() + 1, 
			    note->get_step() + length, 
			    note->get_key()))
      new_length = iter->get_step() - note->get_step();
    
    // remove the note off event
    // must be threadsafe!
    NoteEvent* previous = note->m_note_off->get_previous();
    NoteEvent* next = static_cast<NoteEvent*>(note->m_note_off->get_next());
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
    
    signal_note_changed(note->get_step(), note->get_key(),
			note->get_length());
    
    return note->get_length();
  }


  void Pattern::set_velocity(NoteIterator iterator, unsigned char velocity) {
    assert(iterator);
    assert(iterator.m_pattern == this);
    assert(velocity < 128);
    
    iterator.m_note->m_note_on->set_velocity(velocity);
    
    signal_note_changed(iterator->get_step(), iterator->get_key(),
			iterator->get_length());
  }


  Pattern::ControllerIterator Pattern::add_controller(const std::string& name,
						      unsigned long param, 
						      int min, int max) {
    // find the place to insert the new controller
    unsigned i;
    for (i = 0; i < m_sd->ctrls->size(); ++i) {
      unsigned long this_param = (*m_sd->ctrls)[i]->get_param();
      if (this_param == param)
	return ControllerIterator(m_sd->ctrls->begin() + i);
      else if (this_param > param)
	break;
    }
    
    // make a copy of the controller vector and insert the new one
    vector<Controller*>* new_vector = new vector<Controller*>(*m_sd->ctrls);
    new_vector->insert(new_vector->begin() + i, 
		       new Controller(name, m_sd->steps * m_sd->length, 
				      param, min, max));
    
    // delete the old vector
    vector<Controller*>* tmp = m_sd->ctrls;
    m_sd->ctrls = new_vector;
    Deleter::queue(tmp);
    
    signal_controller_added(param);
    return ControllerIterator(new_vector->begin() + i);
  }
  
  
  void Pattern::remove_controller(ControllerIterator iter) {
    // find the element to erase
    unsigned i;
    for (i = 0; i < m_sd->ctrls->size(); ++i) {
      if ((*m_sd->ctrls)[i] == &*iter)
	break;
    }
    if (i >= m_sd->ctrls->size())
      return;
    
    // make a copy of the old vector
    vector<Controller*>* new_vector = new vector<Controller*>(*m_sd->ctrls);
    unsigned long param = (*m_sd->ctrls)[i]->get_param();
    new_vector->erase(new_vector->begin() + i);
    
    // delete the old vector
    vector<Controller*>* tmp = m_sd->ctrls;
    m_sd->ctrls = new_vector;
    Deleter::queue(tmp);
    
    signal_controller_removed(param);
  }

  
  void Pattern::add_cc(ControllerIterator iter, unsigned int step, 
		       unsigned char value) {
    assert(step < m_sd->length * m_sd->steps);
    const CCEvent* e = (*iter.m_iterator)->get_event(step);
    (*iter.m_iterator)->set_event(step, value);
    if (!e || e->get_value() != value)
      signal_cc_added((*iter.m_iterator)->get_param(), step, value);
  }


  void Pattern::remove_cc(ControllerIterator iter, unsigned int step) {
    assert(step < m_sd->length * m_sd->steps);
    const CCEvent* e = (*iter.m_iterator)->get_event(step);
    (*iter.m_iterator)->remove_event(step);
    if (e)
      signal_cc_removed((*iter.m_iterator)->get_param(), step);
  }

  
  int Pattern::get_steps() const {
    return m_sd->steps;
  }


  int Pattern::get_length() const {
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
    // we need a local copy of the SeqData pointer so the other thread
    // doesn't change it before we are done
    SeqData* sd = m_sd;
    
    assert(beat < sd->length);
    assert(before_beat <= sd->length);
    
    int list_no = 0;
    double off_d = 0.001;
    unsigned step;
    for (step = unsigned(ceil(beat * sd->steps));
	 step < before_beat * sd->steps; ++step) {
      
      // note off events just before this step
      if (step > 0 && (*sd->offs)[step-1] && 
	  sd->steps * (beat + off_d) <= step) {
	events[list_no] = (*sd->offs)[step - 1];
	beats[list_no] = step / double(sd->steps) - off_d;
	if (++list_no == room) break;
      }
      
      // note on events on this step
      if ((*sd->ons)[step]) {
	events[list_no] = (*sd->ons)[step];
	beats[list_no] = step / double(sd->steps);
	if (++list_no == room) break;
      }
      
      // controllers
      for (unsigned i = 0; i < m_sd->ctrls->size(); ++i) {
	if ((*m_sd->ctrls)[i]->get_event(step)) {
	  events[list_no] = (*m_sd->ctrls)[i]->get_event(step);
	  beats[list_no] = step / double(sd->steps);
	  if (++list_no == room) break;
	}
      }
      if (list_no == room) break;
      
    }
    
    // note off events after the last step
    if (sd->steps * (before_beat + off_d) > step && (*sd->offs)[step - 1] &&
	list_no < room) {
      events[list_no] = (*sd->offs)[step - 1];
      beats[list_no] = step / double(sd->steps) - off_d;
      ++list_no;
    }
    
    return list_no;
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
	    return NoteIterator(this, NULL);
	}
	note_on = static_cast<NoteEvent*>(note_on->get_next());
      }
      
    }
    
    return NoteIterator(this, NULL);
  }


  Pattern::ControllerIterator Pattern::ctrls_begin() const {
    return ControllerIterator(m_sd->ctrls->begin());
  }
  
  
  Pattern::ControllerIterator Pattern::ctrls_end() const {
    return ControllerIterator(m_sd->ctrls->end());
  }
  
  
  Pattern::ControllerIterator Pattern::ctrls_find(unsigned long param) const {
    for (unsigned i = 0; i < m_sd->ctrls->size(); ++i) {
      if ((*m_sd->ctrls)[i]->get_param() == param)
	return ControllerIterator(m_sd->ctrls->begin() + i);
    }
    return ctrls_end();
  }

  
  Pattern::NoteIterator Pattern::find_note_on(unsigned start, unsigned end, 
					      unsigned char key) {
    for (unsigned i = start; i < end; ++i) {
      NoteEvent* note_on = (*m_sd->ons)[i];
      while (note_on) {
	if (note_on->get_key() == key)
	  return NoteIterator(this, note_on->get_note());
	note_on = static_cast<NoteEvent*>(note_on->get_next());
      }
    }
    
    return NoteIterator(this, NULL);
  }
  

}


