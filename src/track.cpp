#include <cstdio>
#include <iostream>

#include "track.hpp"


Track::Track(int length) 
  : m_length(length), m_dirty(false), m_current_seq_entry(NULL) {

}


/** Returns the name of the track. The name is just a label that the user
    can change. */
const string& Track::get_name() const {
  return m_name;
}


/** Returns a map of the patterns in this track. This map can be changed
    by the user, and we have no way of knowing how it's changed - this will
    probably be replaced by higher level functions in the future. */
map<int, Pattern>& Track::get_patterns() {
  return m_patterns;
}


const map<int, Pattern>& Track::get_patterns() const {
  return m_patterns;
}


/** Returns the sequence as a map of ints indexed by ints. This will probably
    be removed or made const in the future. */
Track::Sequence& Track::get_sequence() {
  return m_sequence;
}


/** Return the SequenceEntry that is playing at the given beat, or -1. */
bool Track::get_sequence_entry(int at, int& beat, 
			       int& pattern, int& length) const {
  for (Sequence::const_iterator iter = m_sequence.begin(); 
       iter != m_sequence.end() && iter->first <= at; ++iter) {
    if (iter->first + iter->second->length > at) {
      beat = iter->first;
      pattern = iter->second->pattern_id;
      length = iter->second->length;
      return true;
    }
  }
  return false;
}


/** Creates and adds a new pattern in this track with the given parameters.*/
int Track::add_pattern(int length, int steps, int ccSteps) {
  int id;
  if (m_patterns.rbegin() != m_patterns.rend())
    id = m_patterns.rbegin()->first + 1;
  else
    id = 1;
  m_patterns[id] = Pattern(length, steps, ccSteps);
  signal_pattern_added(id);
  return id;
}


/** Set the sequency entry at the given beat to the given pattern and length.*/
void Track::set_sequence_entry(int beat, int pattern, int length) {
  
  assert(beat >= 0);
  assert(beat < m_length);
  assert(m_patterns.find(pattern) != m_patterns.end());
  assert(length > 0 || length == -1);
  assert(length <= m_patterns.find(pattern)->second.get_length());
  
  // if length is -1, get it from the pattern
  int newLength;
  if (length == -1)
    newLength = m_patterns[pattern].get_length();
  else
    newLength = length;
  
  SequenceEntry* previous = NULL;
  SequenceEntry* next = NULL;
  
  Sequence::iterator iter;
  Sequence::iterator toUpdate = m_sequence.end();
  bool isUpdate = false;
  for (iter = m_sequence.begin(); iter != m_sequence.end(); ++iter) {
    
    // if a pattern is playing at the given beat, shorten it so it stops just
    // before that beat
    if (iter->first < beat) {
      previous = iter->second;
      if (iter->first + iter->second->length > beat)
	iter->second->length = beat - iter->first;
    }
    // if there already is a pattern starting at this beat, erase it unless 
    // it's the same pattern, in which case we just remember it for later
    else if (iter->first == beat) {
      isUpdate = true;
      if (iter->second->pattern_id == pattern)
	toUpdate = iter;
      else
	m_sequence.erase(iter);
    }
    
    // we've found the next sequence entry - remember it as "next" and stop
    // also make sure that the new sequence entry doesn't overlap with this one
    else if (iter->first > beat) {
      next = iter->second;
      if (iter->first < beat + newLength)
	newLength = iter->first - beat;
      break;
    }
  }
  
  // force the pattern to stop at the end of the song
  if (beat + newLength > m_length)
    newLength = m_length - beat;
  
  // if the pattern was there already, just update the length
  if (toUpdate != m_sequence.end()) {
    toUpdate->second->length = newLength;
    return;
  }
  
  else {
    // setup the linked list and add the new entry
    SequenceEntry* entry = new SequenceEntry(pattern, 
					     &m_patterns.find(pattern)->second, 
					     beat, newLength);
    entry->next = next;
    entry->previous = previous;
    if (next)
      next->previous = entry;
    if (previous)
      previous->next = entry;
    m_sequence[beat] = entry;
    
    if (isUpdate)
      signal_sequence_entry_changed(beat, pattern, newLength);
    else
      signal_sequence_entry_added(beat, pattern, newLength);
  }
}


/** Remove the sequence entry (pattern) that is playing at the given beat.
    If no pattern is playing at that beat, return @c false. */
bool Track::remove_sequence_entry(int beat) {
  Sequence::iterator iter = m_sequence.begin();
  for ( ; iter != m_sequence.end(); ++iter) {
    if (iter->first <= beat && iter->first + iter->second->length > beat) {
      if (iter->second->previous)
	iter->second->previous->next = iter->second->next;
      if (iter->second->next)
	iter->second->next->previous = iter->second->previous;
      int r_beat = iter->second->start;
      delete iter->second;
      m_sequence.erase(iter);
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


/** Has the track been changed since the last makeClean()? */
bool Track::is_dirty() const {
  if (m_dirty)
    return true;
  for (map<int, Pattern>::const_iterator iter = m_patterns.begin();
       iter != m_patterns.end(); ++iter) {
    if (iter->second.is_dirty())
      return true;
  }
  return false;
}

/** Reset the dirty flag. */
void Track::make_clean() const {
  m_dirty = false;
  for (map<int, Pattern>::const_iterator iter = m_patterns.begin();
       iter != m_patterns.end(); ++iter)
    iter->second.make_clean();
}


/** Get the next note event that occurs before the given time, or return 
    false if there isn't one. This function <b>must be realtime safe</b>
    because it is used by the sequencer thread. */
bool Track::get_next_note(int& beat, int& tick, int& value, int& length,
			  int beforeBeat, int beforeTick) const {
  // no patterns left to play
  if (!m_current_seq_entry)
    return false;
  
  // convert beats and ticks to steps
  const Pattern* pat = m_current_seq_entry->pattern;
  int steps = pat->get_steps();
  int bStep = (beforeBeat - m_current_seq_entry->start) * steps + 
    int(beforeTick * steps / 10000.0);
  int totalLength = pat->get_steps() * m_current_seq_entry->length;
  bStep = (bStep < totalLength ? bStep : totalLength);
  
  // while loops are scary in SCHED_FIFO...
  int step;
  while (m_current_seq_entry && 
	 !m_current_seq_entry->pattern->get_next_note(step, value, length, bStep)) {
    // nothing found, and we don't want notes from next pattern
    if (m_current_seq_entry->start + m_current_seq_entry->length > beforeBeat)
      return false;
    // we might want notes from next pattern, if it starts early enough
    else if (m_current_seq_entry->next && 
	     m_current_seq_entry->next->start <= beforeBeat) {
      m_current_seq_entry = m_current_seq_entry->next;
      if (m_current_seq_entry) {
	m_current_seq_entry->pattern->find_next_note(0);
	pat = m_current_seq_entry->pattern;
	steps = pat->get_steps();
	bStep = (beforeBeat - m_current_seq_entry->start) * steps + 
	  int(beforeTick * steps / 10000.0);
	totalLength = pat->get_steps() * m_current_seq_entry->length;
	bStep = (bStep < totalLength ? bStep : totalLength);
      }
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
  Sequence::const_iterator iter;
  for (iter = m_sequence.begin(); iter != m_sequence.end(); ++iter) {
    if (iter->first <= beat && iter->first + iter->second->length > beat)
      break;
  }
  if (iter == m_sequence.end())
    m_current_seq_entry == NULL;
  else {
    m_current_seq_entry = iter->second;
    int steps = m_current_seq_entry->pattern->get_steps();
    int step = (beat - iter->first) * steps + int(tick * steps / 10000.0);
    m_current_seq_entry->pattern->find_next_note(step);
  }
}


/** Serialize this track to a XML node and return it. */
xmlNodePtr Track::get_xml_node(xmlDocPtr doc) const {
  xmlNodePtr node = xmlNewDocNode(doc, NULL, xmlCharStrdup("track"), NULL);
  char tmpStr[20];
  
  // the patterns
  for (map<int, Pattern>::const_iterator iter = m_patterns.begin();
       iter != m_patterns.end(); ++iter) {
    xmlNodePtr child = iter->second.get_xml_node(doc);
    sprintf(tmpStr, "%d", iter->first);
    xmlNewProp(child, xmlCharStrdup("id"), xmlCharStrdup(tmpStr));
    xmlAddChild(node, child);
  }
  
  // the sequence
  xmlNodePtr seq = xmlNewDocNode(doc, NULL, xmlCharStrdup("sequence"), NULL);
  xmlAddChild(node, seq);
  for (Sequence::const_iterator iter = m_sequence.begin();
       iter != m_sequence.end(); ++iter) {
    xmlNodePtr entry = xmlNewDocNode(doc, NULL, xmlCharStrdup("entry"), NULL);
    sprintf(tmpStr, "%d", iter->first);
    xmlNewProp(entry, xmlCharStrdup("beat"), xmlCharStrdup(tmpStr));
    sprintf(tmpStr, "%d", iter->second->pattern_id);
    xmlNewProp(entry, xmlCharStrdup("pattern"), xmlCharStrdup(tmpStr));
    sprintf(tmpStr, "%d", iter->second->length);
    xmlNewProp(entry, xmlCharStrdup("length"), xmlCharStrdup(tmpStr));
    xmlAddChild(seq, entry);
  }
  
  return node;
}
