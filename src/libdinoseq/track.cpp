#include <cstdio>
#include <iostream>

#include "debug.hpp"
#include "deleter.hpp"
#include "midievent.hpp"
#include "pattern.hpp"
#include "track.hpp"


namespace Dino {


  Track::Track(int length, const string& name) 
    : m_name(name), m_length(length), m_dirty(false) {
  
    dbg1<<"Creating track \""<<name<<"\""<<endl;
    for (int i = 0; i < 300; ++i)
      m_sequence.push_back(NULL);
  }


  Track::~Track() {
    dbg1<<"Destroying track \""<<m_name<<"\""<<endl;
    map<int, Pattern*>::iterator iter;
    for (iter = m_patterns.begin(); iter != m_patterns.end(); ++iter)
      delete iter->second;
    SequenceEntry* ptr = NULL;
    for (unsigned int i = 0; i < m_sequence.size(); ++i) {
      if (m_sequence[i] != ptr)
	delete m_sequence[i];
      ptr = m_sequence[i];
    }
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
  /*const vector<Track::SequenceEntry*>& Track::get_sequence() const {
    return vector<const SequenceEntry*>(m_sequence);
    }
  */

  const Track::SequenceEntry* Track::get_seq_entry(unsigned int beat) {
    assert(beat < m_sequence.size());
    return m_sequence[beat];
  }


  /** Returns the MIDI channel for this track. */
  int Track::get_channel() const {
    return m_channel;
  }


  unsigned int Track::get_length() const {
    return m_sequence.size();
  }


  /** Sets the name of this track. */
  void Track::set_name(const string& name) {
    if (name != m_name) {
      dbg1<<"Changing track name from \""<<m_name<<"\" to \""<<name<<"\""<<endl;
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

  
  /** Removes the pattern with the given ID. */
  void Track::remove_pattern(int id) {
    // XXX Make sure that this is threadsafe
    map<int, Pattern*>::iterator iter = m_patterns.find(id);
    if (iter != m_patterns.end()) {
      g_deletable_deleter.queue_deletion(iter->second);
      m_patterns.erase(iter);
      signal_pattern_removed(id);
    }
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
  
    // delete or shorten any sequence entry at this beat
    SequenceEntry* se = m_sequence[beat];
    if (se) {
      int stop = se->start + se->length;
      if (se->start == unsigned(beat))
	delete se;
      else
	se->length = beat - se->start;
      for (int i = stop - 1; i >= beat; --i)
	m_sequence[i] = NULL;
    }
  
    // make sure the new sequence entry fits
    for (int i = beat; i < beat + newLength; ++i) {
      if (m_sequence[i]) {
	newLength = i - beat;
	break;
      }
    }
  
    se  = new SequenceEntry(pattern, m_patterns.find(pattern)->second, 
			    beat, newLength);
    for (int i = beat; i < beat + newLength; ++i)
      m_sequence[i] = se;
  }


  void Track::set_seq_entry_length(unsigned int beat, unsigned int length) {
    assert(beat < m_sequence.size());
    assert(beat + length <= m_sequence.size());
    assert(m_sequence[beat]);
    SequenceEntry* se = m_sequence[beat];
    if (length == se->length)
      return;
    else if (length > se->length) {
      unsigned int i;
      for (i = se->start + se->length; i < se->start + length; ++i) {
	if (m_sequence[i])
	  break;
	m_sequence[i] = se;
      }
      se->length = i - se->start;
    }
    else {
      int tmp = se->length;
      se->length = length;
      for (int i = se->start + tmp - 1; i >= int(se->start + se->length); --i)
	m_sequence[i] = NULL;
    }
    // XXX signal here?
  }



  /** Remove the sequence entry (pattern) that is playing at the given beat.
      If no pattern is playing at that beat, return @c false. */
  bool Track::remove_sequence_entry(int beat) {
    assert(beat >= 0);
    assert(beat < m_length);
    SequenceEntry* se = m_sequence[beat];
    if (se) {
      for (int i = se->start + se->length - 1; i >= int(se->start); --i)
	m_sequence[i] = NULL;
      delete se;
      return true;
    }
    return false;
  }


  /** Set the length of the track. Only the Song should do this. */
  void Track::set_length(int length) {
    // XXX delete and shorten sequence entries to fit the new length
    // and make sure that it's threadsafe!
    if (length != m_length) {
      m_length = length;
      signal_length_changed(m_length);
    }
  }


  /** Set the MIDI channel for this track. */
  void Track::set_channel(int channel) {
    dbg1<<"Setting MIDI channel for track \""<<m_name<<"\" to "<<channel<<endl;
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
    for (unsigned int i = 0; i < m_sequence.size(); ++i) {
      SequenceEntry* se = m_sequence[i];
      if (se && se->start == i) {
	Element* entry_elt = seq_elt->add_child("entry");
	sprintf(tmp_txt, "%d", se->start);
	entry_elt->set_attribute("beat", tmp_txt);
	sprintf(tmp_txt, "%d", se->pattern_id);
	entry_elt->set_attribute("pattern", tmp_txt);
	sprintf(tmp_txt, "%d", se->length);
	entry_elt->set_attribute("length", tmp_txt);
      }
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


  MIDIEvent* Track::get_events(unsigned int& beat, unsigned int& tick, 
			       unsigned int before_beat, unsigned int before_tick,
			       unsigned int ticks_per_beat,
			       unsigned int& list) const {
  
    // if we have reached the end of the wanted period, return NULL so the
    // sequencer will stop looking for events in this track
    if (beat >= before_beat && tick >= before_tick)
      return NULL;
  
    // look in all beats inside this period
    for (unsigned int i = beat; i <= before_beat; ++i) {
      if (i == before_beat && before_tick == 0)
	break;
      SequenceEntry* se = m_sequence[i];
      if (se) {
	beat -= se->start;
	unsigned int btick = before_tick;
	if (before_beat - se->start >= se->length)
	  btick = 0;
	MIDIEvent* event = se->pattern->get_events(beat, tick, 
						   before_beat- se->start,
						   btick, ticks_per_beat,
						   list);
	beat += se->start;
	if (event)
	  return event;
	if (beat >= se->start + se->length) {
	  return &MIDIEvent::AllNotesOff;
	}
      }
    }
  
    return NULL;
  }


}
