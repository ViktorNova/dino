#include <algorithm>
#include <cassert>
#include <iostream>

#include "pattern.hpp"


Pattern::Pattern() : m_dirty(false), m_next_note(NULL), m_name("Untitled") {
  
}


Pattern::Pattern(int length, int steps, int ccSteps) 
  : m_dirty(false), m_next_note(NULL), m_name("Untitled") {
  this->m_length = length;
  this->m_steps = steps;
  this->m_cc_steps = ccSteps;
  m_min_step = m_length * m_steps;
  m_max_step = -1;
  m_min_note = 128;
  m_max_note = -1;
}


const string& Pattern::get_name() const {
  return m_name;
}


/** This function tries to add a new note with the given parameters. If 
    there is another note starting at the same step and with the same value,
    the old note will be deleted. If there is a note that is playing at that
    step with the same value, the old note will be shortened so it ends just
    before @c step. This function will also update the @c previous and 
    @c next pointers in the Note, so the doubly linked list will stay
    consistent with the note map. */
void Pattern::add_note(int step, int value, int noteLength) {
  assert(step >= 0 && step < m_length * m_steps);
  assert(value >= 0 && value < 128);
  assert(noteLength > 0);
  assert(step + noteLength <= m_length * m_steps);
  pair<int, int> p = make_pair(step, value);
  list<Note>::iterator iter;
  
  bool isUpdate = false;
  
  for (iter = m_notes.begin(); iter != m_notes.end(); ++iter) {
    
    // we're looking at a note that's before the new one
    if (iter->step < step || 
	(iter->step == step && iter->value < value)) {
      // it could be overlapping, if so, shorten it
      if (iter->value == value && iter->step + iter->length > step) {
	m_max_step = (iter->step + iter->length > m_max_step ? 
		      iter->step + iter->length : m_max_step);
	iter->length = step - iter->step;
      }
    }
    
    // we're looking at a note with the same pitch and start time - remove it
    else if (iter->step == step && iter->value == value) {
      isUpdate = true;
      m_max_step = (iter->step + iter->length > m_max_step ?
		    iter->step + iter->length : m_max_step);
      m_notes.erase(iter--);
    }
    
    // we're looking at a node after the new one - exit the loop
    else if (iter->step > step || (iter->step == step && iter->value > value))
      break;
  }
  
  // check if there is a note that begins before the end of the new note,
  // if so shorten the new note
  list<Note>::iterator next = iter;
  for ( ; next != m_notes.end(); ++next) {
    if (next->value == value) {
      if (next->step > step && step + noteLength > next->step)
	noteLength = next->step - step;
      break;
    }
  }
  
  // create the note, set up links for the sequencer list and put it in the
  // map
  Note note(p.first, p.second, noteLength);
  m_notes.insert(iter, note);
  
  m_min_note = value < m_min_note ? value : m_min_note;
  m_max_note = value > m_max_note ? value : m_max_note;
  m_min_step = step < m_min_step ? step : m_min_step;
  m_max_step = step + noteLength - 1 > m_max_step ? step + noteLength - 1 : m_max_step;
  
  if (isUpdate)
    signal_note_changed(p.first, p.second, noteLength);
  else
    signal_note_added(p.first, p.second, noteLength);
}


/** This function will delete the note with the given value playing at the
    given step, if there is one. It will also update the @c previous and 
    @c next pointers so the doubly linked list will stay consistent with the
    note map. It will return the step that the deleted note started on,
    or -1 if no note was deleted. */
int Pattern::delete_note(int step, int value) {
  assert(step >= 0 && step < m_length * m_steps);
  assert(value >= 0 && value < 128);
  for (list<Note>::iterator iter = m_notes.begin(); 
       iter != m_notes.end() && iter->step <= step; ++iter) {
    if (iter->value == value && iter->step <= step &&
	iter->step + iter->length > step) {
      int result = iter->step;
      m_min_note = iter->value < m_min_note ? iter->value : m_min_note;
      m_max_note = iter->value > m_max_note ? iter->value : m_max_note;
      m_min_step = iter->step < m_min_step ? iter->step : m_min_step;
      m_max_step = (iter->step + iter->length - 1 > m_max_step ?
		    iter->step + iter->length - 1 : m_max_step);
      int rStep = iter->step;
      int rNote = iter->value;
      m_notes.erase(iter);
      signal_note_removed(rStep, rNote);
      return result;
    }
  }
  return -1;
}


void Pattern::add_cc(int ccNumber, int step, int value) {
  assert(ccNumber >= 0 && ccNumber < 128);
  assert(step >= 0 && step < m_length * m_cc_steps);
  assert(value >= 0 && value < 128);
  bool isUpdate = false;
  if (m_control_changes[ccNumber].changes.find(step) != 
      m_control_changes[ccNumber].changes.end())
    isUpdate = true;
  m_control_changes[ccNumber].changes[step] = new CCEvent(ccNumber, value);
  if (isUpdate)
    signal_cc_changed(ccNumber, step, value);
  else
    signal_cc_added(ccNumber, step, value);
}


int Pattern::delete_cc(int ccNumber, int step) {
  assert(ccNumber >= 0 && ccNumber < 128);
  assert(step >= 0 && step < m_length * m_cc_steps);
  CCData& data(m_control_changes[ccNumber]);
  map<int, CCEvent*>::iterator iter = data.changes.find(step);
  if (iter != data.changes.end()) {
    delete iter->second;
    data.changes.erase(iter);
    signal_cc_removed(ccNumber, step);
    return step;
  }
  return -1;
}


list<Pattern::Note>& Pattern::get_notes() {
  return m_notes;
}


const list<Pattern::Note>& Pattern::get_notes() const {
  return m_notes;
}


Pattern::CCData& Pattern::get_cc(int ccNumber) {
  assert(ccNumber >= 0 && ccNumber < 128);
  return m_control_changes[ccNumber];
}


int Pattern::get_steps() const {
  return m_steps;
}


int Pattern::get_cc_steps() const {
  return m_cc_steps;
}


int Pattern::get_length() const {
  return m_length;
}
  

void Pattern::get_dirty_rect(int* minStep, int* minNote, 
			     int* maxStep, int* maxNote) {
  if (minStep)
    *minStep = this->m_min_step;
  if (minNote)
    *minNote = this->m_min_note;
  if (maxStep)
    *maxStep = this->m_max_step;
  if (maxNote)
    *maxNote = this->m_max_note;
  this->m_min_step = m_length * m_steps;
  this->m_max_step = -1;
  this->m_min_note = 128;
  this->m_max_note = -1;
}


/** This function will fill in the step, value, and length of the next note
    event that occurs before @c beforeStep, if there is one. If there isn't
    it will return false. It <b>must be realtime safe</b> because it is 
    used by the sequencer thread. */
bool Pattern::get_next_note(int& step, int& value,int& length, 
			    int beforeStep) const{
  // no notes left in the pattern
  if (m_next_note == m_notes.end())
    return false;
  
  // this is the first call since a find_next_note(), so use the current note
  if (m_first_note && m_next_note->step < beforeStep) {
    m_first_note = false;
    step = m_next_note->step;
    value = m_next_note->value;
    length = m_next_note->length;
    return true;
  }
  
  // this is not the first call, so go to next note and use that
  list<Note>::const_iterator next = m_next_note;
  ++next;
  if (!m_first_note && next != m_notes.end() && next->step < beforeStep) {
    m_next_note = next;
    step = m_next_note->step;
    value = m_next_note->value;
    length = m_next_note->length;
    return true;
  }
  
  return false;
}


/** This function sets "next note" to the first note at or after the given
    step. It <b>must be realtime safe</b> for @c step == 0 because that is
    used by the sequencer thread. */
void Pattern::find_next_note(int step) const {
  list<Note>::const_iterator iter = m_notes.begin();
  for ( ; iter != m_notes.end() && iter->step >= step; ++iter);
  m_next_note = iter;
  m_first_note = true;
}


bool Pattern::is_dirty() const {
  return m_dirty;
}


void Pattern::make_clean() const {
  m_dirty = false;
}


bool Pattern::fill_xml_node(Element* elt) const {
  char tmp_txt[10];
  elt->add_child("name")->set_child_text(m_name);
  sprintf(tmp_txt, "%d", get_length());
  elt->set_attribute("length", tmp_txt);
  sprintf(tmp_txt, "%d", get_steps());
  elt->set_attribute("steps", tmp_txt);
  sprintf(tmp_txt, "%d", get_cc_steps());
  elt->set_attribute("ccsteps", tmp_txt);
  for (list<Note>::const_iterator iter = m_notes.begin(); 
       iter != m_notes.end(); ++iter) {
    Element* note_elt = elt->add_child("note");
    sprintf(tmp_txt, "%d", iter->step);
    note_elt->set_attribute("step", tmp_txt);
    sprintf(tmp_txt, "%d", iter->value);
    note_elt->set_attribute("value", tmp_txt);
    sprintf(tmp_txt, "%d", iter->length);
    note_elt->set_attribute("length", tmp_txt);
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

  char tmp_txt[10];
  nodes = elt->get_children("note");
  Node::NodeList::const_iterator iter;
  for (iter = nodes.begin(); iter != nodes.end(); ++iter) {
    const Element* note_elt = dynamic_cast<const Element*>(*iter);
    if (!note_elt)
      continue;
    int step, value, length;
    sscanf(note_elt->get_attribute("step")->get_value().c_str(), "%d", &step);
    sscanf(note_elt->get_attribute("value")->get_value().c_str(), 
	   "%d", &value);
    sscanf(note_elt->get_attribute("length")->get_value().c_str(), 
	   "%d", &length);
    add_note(step, value, length);
  }
  return true;
}
