#include <cstdio>
#include <iostream>

#include "track.hpp"


Track::Track(int length, const string& name) 
  : m_length(length), m_dirty(false), m_name(name) {
  m_seq_head = new SequenceEntry(-1, NULL, -1, 0);
  m_current_seq_entry = m_seq_head;
}


/** Returns the name of the track. The name is just a label that the user
    can change. */
const string& Track::get_name() const {
  return m_name;
}


/** Returns a map of the patterns in this track. This map can be changed
    by the user, and we have no way of knowing how it's changed - this will
    probably be replaced by higher level functions in the future. */
map<int, Pattern*>& Track::get_patterns() {
  return m_patterns;
}


const map<int, Pattern*>& Track::get_patterns() const {
  return m_patterns;
}


/** Returns the sequence as a map of ints indexed by ints. This will probably
    be removed or made const in the future. */
Track::SequenceEntry* Track::get_sequence() {
  return m_seq_head->next;
}


/** Return the SequenceEntry that is playing at the given beat, or -1. */
bool Track::get_sequence_entry(int at, int& beat, 
			       int& pattern, int& length) const {
  for (const SequenceEntry* iter = m_seq_head; 
       iter && iter->start <= at; iter = iter->next) {
    if (iter->start + iter->length > at) {
      beat = iter->start;
      pattern = iter->pattern_id;
      length = iter->length;
      return true;
    }
  }
  return false;
}


/** Returns the MIDI channel for this track. */
int Track::get_channel() const {
  return m_channel;
}


/** Sets the name of this track. */
void Track::set_name(const string& name) {
  if (name != m_name) {
    m_name = name;
    signal_name_changed(m_name);
  }
}


/** Creates and adds a new pattern in this track with the given parameters.*/
int Track::add_pattern(const string& name, int length, 
		       int steps, int ccSteps) {
  int id;
  if (m_patterns.rbegin() != m_patterns.rend())
    id = m_patterns.rbegin()->first + 1;
  else
    id = 1;
  m_patterns[id] = new Pattern(name, length, steps, ccSteps);
  signal_pattern_added(id);
  return id;
}


/** Set the sequency entry at the given beat to the given pattern and length.*/
void Track::set_sequence_entry(int beat, int pattern, int length) {
  assert(beat >= 0);
  assert(beat < m_length);
  assert(m_patterns.find(pattern) != m_patterns.end());
  assert(length > 0 || length == -1);
  assert(length <= m_patterns.find(pattern)->second->get_length());
  
  // if length is -1, get it from the pattern
  int newLength;
  if (length == -1)
    newLength = m_patterns[pattern]->get_length();
  else
    newLength = length;
  
  SequenceEntry* iter;
  SequenceEntry* position = m_seq_head;
  bool is_update = false;
  for (iter = m_seq_head; iter && iter->start <= beat; iter = iter->next) {
    
    position = iter;
    
    // if a pattern is playing at the given beat, shorten it so it stops just
    // before that beat
    if (iter->start < beat && iter->start + iter->length > beat)
      iter->length = beat - iter->start;

    // if there already is a pattern starting at this beat, erase it unless 
    // it's the same pattern, in which case we just remember it for later
    else if (iter->start == beat) {
      if (iter->pattern_id == pattern)
	is_update = true;
      else {
	SequenceEntry* to_be_deleted = iter;
	iter = iter->prev;
	position = iter;
	iter->next = to_be_deleted->next;
	if (to_be_deleted->next)
	  to_be_deleted->next->prev = iter;
	Mutex::Lock lock(m_lock);
	if (to_be_deleted == m_current_seq_entry)
	  m_current_seq_entry = m_current_seq_entry->prev;
	lock.release();
	delete to_be_deleted;
      }
      break;
    }
  }
  
  // make sure that the new sequence entry doesn't overlap with the next one
  if (position->next && position->next->start < beat + newLength)
      newLength = position->next->start - beat;
  
  // force the pattern to stop at the end of the song
  if (beat + newLength > m_length)
    newLength = m_length - beat;
  
  // insert the new entry
  if (!is_update) {
    SequenceEntry* entry = new SequenceEntry(pattern, 
					     m_patterns.find(pattern)->second, 
					     beat, newLength);
    entry->next = position->next;
    entry->prev = position;
    
    Mutex::Lock lock(m_lock);
    position->next = entry;
    lock.release();
    
    if (entry->next)
      entry->next->prev = entry;
    signal_sequence_entry_added(beat, pattern, newLength);  
  }
  else {
    position->length = newLength;
    signal_sequence_entry_changed(beat, pattern, newLength);
  }
}


/** Remove the sequence entry (pattern) that is playing at the given beat.
    If no pattern is playing at that beat, return @c false. */
bool Track::remove_sequence_entry(int beat) {
  for (  SequenceEntry* iter = m_seq_head; iter; iter = iter->next) {
    if (iter->start <= beat && iter->start + iter->length > beat) {
      int r_beat = iter->start;
      iter->prev->next = iter->next;
      if (iter->next)
	iter->next->prev = iter->prev;
      Mutex::Lock lock(m_lock);
      if (m_current_seq_entry == iter)
	m_current_seq_entry = iter->prev;
      lock.release();
      delete iter;
      signal_sequence_entry_removed(r_beat);
      return true;
    }
  }
  return false;
}


/** Set the length of the track. Only the Song should do this. */
void Track::set_length(int length) {
  if (length != m_length) {
    m_length = length;
    signal_length_changed(m_length);
  }
}


/** Set the MIDI channel for this track. */
void Track::set_channel(int channel) {
  m_channel = channel;
}


/** Has the track been changed since the last makeClean()? */
bool Track::is_dirty() const {
  if (m_dirty)
    return true;
  for (map<int, Pattern*>::const_iterator iter = m_patterns.begin();
       iter != m_patterns.end(); ++iter) {
    if (iter->second->is_dirty())
      return true;
  }
  return false;
}

/** Reset the dirty flag. */
void Track::make_clean() const {
  m_dirty = false;
  for (map<int, Pattern*>::const_iterator iter = m_patterns.begin();
       iter != m_patterns.end(); ++iter)
    iter->second->make_clean();
}


/** Get the next note event that occurs before the given time, or return 
    false if there isn't one. This function <b>must be realtime safe</b>
    because it is used by the sequencer thread. */
bool Track::get_next_note(int& beat, int& tick, int& value, int& length,
			  int beforeBeat, int beforeTick) const {
  Mutex::Lock lock(m_lock);
  // no patterns left to play
  if (!m_current_seq_entry)
    return false;
  
  // convert beats and ticks to steps
  const Pattern* pat = m_current_seq_entry->pattern;
  int steps = pat ? pat->get_steps() : 0;
  int bStep = (beforeBeat - m_current_seq_entry->start) * steps + 
    int(beforeTick * steps / 10000.0);
  int totalLength = steps * m_current_seq_entry->length;
  bStep = (bStep < totalLength ? bStep : totalLength);
  
  // while loops are scary in SCHED_FIFO...
  int step;
  while (m_current_seq_entry) {
    if (m_current_seq_entry != m_seq_head &&
	m_current_seq_entry->pattern->get_next_note(step, value, length,bStep))
      break;
    
    // nothing found, and we don't want notes from next pattern
    if (m_current_seq_entry->start + m_current_seq_entry->length > beforeBeat)
      return false;
    
    // we might want notes from next pattern, if it starts early enough
    SequenceEntry* next = m_current_seq_entry->next;
    if (next && next->start <= beforeBeat) {
      m_current_seq_entry = next;
      m_current_seq_entry->pattern->find_next_note(0);
      pat = m_current_seq_entry->pattern;
      steps = pat->get_steps();
      bStep = (beforeBeat - m_current_seq_entry->start) * steps + 
	int(beforeTick * steps / 10000.0);
      totalLength = pat->get_steps() * m_current_seq_entry->length;
      bStep = (bStep < totalLength ? bStep : totalLength);
    }

    // it doesn't
    else
      return false;
  }
  
  // did we get anything?
  if (m_current_seq_entry) {
    int steps = m_current_seq_entry->pattern->get_steps();
    beat = m_current_seq_entry->start + step / steps;
    tick = int((step % steps) * 10000.0 / steps);
    length = int(length * 10000.0 / steps);
    return true;
  }
  
  return false;
}


/** Get the next CC event, or return false if there isn't one. This 
    function <b>must be realtime safe</b> because it is used by the 
    sequencer thread. */
bool Track::get_next_cc_event(int& step, int& tick, 
			   int& number, int& value) const {
  return false;
}


/** Sets "next note" to the next note after or at the given step. This 
    function does not have to be realtime safe. */  
void Track::find_next_note(int beat, int tick) const {
  Mutex::Lock lock(m_lock);
  cerr<<"find_next_note("<<beat<<", "<<tick<<")"<<endl;
  SequenceEntry* iter;
  for (iter = m_seq_head; iter; iter = iter->next) {
    if (iter->start + iter->length > beat) {
      if (iter->start >= beat)
	iter = iter->prev;
      break;
    }
  }
  m_current_seq_entry = iter;
  if (m_current_seq_entry && m_current_seq_entry != m_seq_head) {
    int steps = m_current_seq_entry->pattern->get_steps();
    int step = (beat - iter->start) * steps + int(tick * steps / 10000.0);
    m_current_seq_entry->pattern->find_next_note(step);
  }
}


/** Serialize this track to a XML node and return it. */
bool Track::fill_xml_node(Element* elt) const {
  
  // MIDI channel
  elt->set_attribute("name", m_name);
  char tmp_txt[10];
  sprintf(tmp_txt, "%d", get_channel());
  elt->set_attribute("channel", tmp_txt);
 
  // the patterns
  for (map<int, Pattern*>::const_iterator iter = m_patterns.begin();
       iter != m_patterns.end(); ++iter) {
    Element* pat_elt = elt->add_child("pattern");
    char id_txt[10];
    sprintf(id_txt, "%d", iter->first);
    pat_elt->set_attribute("id", id_txt);
    iter->second->fill_xml_node(pat_elt);
  }
  
  // the sequence
  Element* seq_elt = elt->add_child("sequence");
  for (const SequenceEntry* iter = m_seq_head->next; iter; iter = iter->next) {
    Element* entry_elt = seq_elt->add_child("entry");
    sprintf(tmp_txt, "%d", iter->start);
    entry_elt->set_attribute("beat", tmp_txt);
    sprintf(tmp_txt, "%d", iter->pattern_id);
    entry_elt->set_attribute("pattern", tmp_txt);
    sprintf(tmp_txt, "%d", iter->length);
    entry_elt->set_attribute("length", tmp_txt);
  }
  
  return true;
}


bool Track::parse_xml_node(const Element* elt) {
  Node::NodeList nodes;
  Node::NodeList::const_iterator iter;
  
  // MIDI channel
  int channel;
  sscanf(elt->get_attribute("channel")->get_value().c_str(), 
	 "%d", &channel);
  m_channel = channel;
  m_name = elt->get_attribute("name")->get_value();
  
  // parse patterns
  nodes = elt->get_children("pattern");
  for (iter = nodes.begin(); iter != nodes.end(); ++iter) {
    const Element* pat_elt = dynamic_cast<const Element*>(*iter);
    if (!pat_elt)
      continue;
    int id, length, steps, cc_steps;
    sscanf(pat_elt->get_attribute("id")->get_value().c_str(), "%d", &id);
    sscanf(pat_elt->get_attribute("length")->get_value().c_str(), 
	   "%d", &length);
    sscanf(pat_elt->get_attribute("steps")->get_value().c_str(), "%d", &steps);
    sscanf(pat_elt->get_attribute("ccsteps")->get_value().c_str(), "%d", 
	   &cc_steps);
    string name = pat_elt->get_attribute("name")->get_value();
    m_patterns[id] = new Pattern(name, length, steps, cc_steps);
    m_patterns[id]->parse_xml_node(pat_elt);
  }
  
  // parse sequence
  nodes = elt->get_children("sequence");
  if (nodes.begin() != nodes.end()) {
    const Element* seq_elt = dynamic_cast<const Element*>(*nodes.begin());
    if (seq_elt) {
      Node::NodeList e_nodes = (*nodes.begin())->get_children("entry");
      Node::NodeList::const_iterator e_iter;
      for (e_iter = e_nodes.begin(); e_iter != e_nodes.end(); ++e_iter) {
	const Element* entry_elt = dynamic_cast<const Element*>(*e_iter);
	if (!entry_elt)
	  continue;
	int beat, pattern, length;
	sscanf(entry_elt->get_attribute("beat")->get_value().c_str(), 
	       "%d", &beat);
	sscanf(entry_elt->get_attribute("pattern")->get_value().c_str(), 
	       "%d", &pattern);
	sscanf(entry_elt->get_attribute("length")->get_value().c_str(), 
	       "%d", &length);
	set_sequence_entry(beat, pattern, length);
      }
    }
  }
  return true;
}


