#include <algorithm>
#include <cassert>
#include <iostream>

#include "pattern.hpp"


Pattern::Pattern() : m_dirty(false), m_next_note(NULL) {
  
}


Pattern::Pattern(int length, int steps, int ccSteps) : m_dirty(false) {
  this->m_length = length;
  this->m_steps = steps;
  this->m_cc_steps = ccSteps;
  m_min_step = m_length * m_steps;
  m_max_step = -1;
  m_min_note = 128;
  m_max_note = -1;
}


void Pattern::add_note(int step, int value, int noteLength) {
  assert(step >= 0 && step < m_length * m_steps);
  assert(value >= 0 && value < 128);
  assert(noteLength > 0);
  assert(step + noteLength <= m_length * m_steps);
  pair<int, int> p = make_pair(step, value);
  Note* previous = NULL;
  Note* next = NULL;
  NoteMap::iterator iter;
  
  bool isUpdate = false;
  
  for (iter = m_notes.begin(); iter != m_notes.end(); ++iter) {
    
    // we're looking at a note that's before the new one
    if (iter->first.first < step || 
	(iter->first.first == step && iter->first.second < value)) {
      previous = iter->second;
      // it could be overlapping, if so, shorten it
      if (iter->first.second == value && 
	  iter->first.first + iter->second->length > step) {
	m_max_step = (iter->first.first + iter->second->length > m_max_step ? 
		   iter->first.first + iter->second->length : m_max_step);
	iter->second->length = step - iter->first.first;
      }
    }
    
    // we're looking at a note with the same pitch and start time - delete it
    else if (iter->first.first == step && iter->first.second == value) {
      isUpdate = true;
      delete iter->second;
      m_max_step = (iter->first.first + iter->second->length > m_max_step ?
		 iter->first.first + iter->second->length : m_max_step);
    }
    
    // we're looking at a node after the new one - link to it and exit the loop
    else if (iter->first.first > step ||
	     (iter->first.first == step && iter->first.second > value)) {
      next = iter->second;
      break;
    }
  }
  
  // check if there is a note that begins before the end of the new note,
  // if so shorten the new note
  for (iter = m_notes.upper_bound(p); iter != m_notes.end(); ++iter) {
    if (iter->first.second == value) {
      if (iter->first.first > step && step + noteLength > iter->first.first)
	noteLength = iter->first.first - step;
      break;
    }
  }
  
  // create the note, set up links for the sequencer list and put it in the
  // map
  Note* note = new Note(p.first, p.second, noteLength);
  note->previous = previous;
  note->next = next;
  if (next)
    next->previous = note;
  if (previous)
    previous->next = note;
  m_notes[p] = note;
  
  m_min_note = value < m_min_note ? value : m_min_note;
  m_max_note = value > m_max_note ? value : m_max_note;
  m_min_step = step < m_min_step ? step : m_min_step;
  m_max_step = step + noteLength - 1 > m_max_step ? step + noteLength - 1 : m_max_step;
  
  if (isUpdate)
    signal_note_changed(p.first, p.second, noteLength);
  else
    signal_note_added(p.first, p.second, noteLength);
}


int Pattern::delete_note(int step, int value) {
  assert(step >= 0 && step < m_length * m_steps);
  assert(value >= 0 && value < 128);
  for (NoteMap::iterator iter = m_notes.begin(); 
       iter != m_notes.end() && iter->first.first <= step; ++iter) {
    if (iter->first.second == value && iter->first.first <= step &&
	iter->first.first + iter->second->length > step) {
      int result = iter->first.first;
      m_min_note = iter->first.second < m_min_note ? 
	iter->first.second : m_min_note;
      m_max_note = iter->first.second > m_max_note ? 
	iter->first.second : m_max_note;
      m_min_step = iter->first.first < m_min_step ? 
	iter->first.first : m_min_step;
      m_max_step = (iter->first.first + iter->second->length - 1 > m_max_step ?
		    iter->first.first + iter->second->length - 1 : m_max_step);
      if (iter->second->previous)
	iter->second->previous->next = iter->second->next;
      if (iter->second->next)
	iter->second->next->previous = iter->second->previous;
      delete iter->second;
      int rStep = iter->first.first;
      int rNote = iter->first.second;
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


Pattern::NoteMap& Pattern::get_notes() {
  return m_notes;
}


const Pattern::NoteMap& Pattern::get_notes() const {
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


bool Pattern::get_next_note(int& step, int& value,int& length, 
			    int beforeStep) const {
  // no notes left in the pattern
  if (!m_next_note)
    return false;
  
  // this is the first call since a find_next_note(), so use the current pointer
  if (m_first_note && m_next_note->step < beforeStep) {
    m_first_note = false;
    step = m_next_note->step;
    value = m_next_note->value;
    length = m_next_note->length;
    return true;
  }
  
  // this is not the first call, so go to next note and use that
  if (!m_first_note && m_next_note->next && m_next_note->next->step < beforeStep) {
    m_next_note = m_next_note->next;
    step = m_next_note->step;
    value = m_next_note->value;
    length = m_next_note->length;
    return true;
  }
  
  return false;
}


void Pattern::find_next_note(int step) const {
  NoteMap::const_iterator iter = m_notes.lower_bound(make_pair(step, 0));
  if (iter == m_notes.end())
    m_next_note = NULL;
  else
    m_next_note = iter->second;
  m_first_note = true;
}


bool Pattern::is_dirty() const {
  return m_dirty;
}


void Pattern::make_clean() const {
  m_dirty = false;
}


xmlNodePtr Pattern::get_xml_node(xmlDocPtr doc) const {
  xmlNodePtr node = xmlNewDocNode(doc, NULL, xmlCharStrdup("pattern"), NULL);
  char tmpStr[20];
  sprintf(tmpStr, "%d", get_length());
  xmlNewProp(node, xmlCharStrdup("length"), xmlCharStrdup(tmpStr));
  sprintf(tmpStr, "%d", get_steps());
  xmlNewProp(node, xmlCharStrdup("steps"), xmlCharStrdup(tmpStr));
  sprintf(tmpStr, "%d", get_cc_steps());
  xmlNewProp(node, xmlCharStrdup("ccsteps"), xmlCharStrdup(tmpStr));
  for (NoteMap::const_iterator iter = m_notes.begin(); iter != m_notes.end();
       ++iter) {
    xmlNodePtr note = xmlNewDocNode(doc, NULL, xmlCharStrdup("note"), NULL);
    sprintf(tmpStr, "%d", iter->first.first);
    xmlNewProp(note, xmlCharStrdup("step"), xmlCharStrdup(tmpStr));
    sprintf(tmpStr, "%d", iter->first.second);
    xmlNewProp(note, xmlCharStrdup("value"), xmlCharStrdup(tmpStr));
    sprintf(tmpStr, "%d", iter->second->length);
    xmlNewProp(note, xmlCharStrdup("length"), xmlCharStrdup(tmpStr));
    xmlAddChild(node, note);
  }
  return node;
}
