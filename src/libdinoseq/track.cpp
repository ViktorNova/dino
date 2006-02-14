#include <cstdio>
#include <iostream>
#include <cassert>

#include "debug.hpp"
#include "deleter.hpp"
#include "midievent.hpp"
#include "pattern.hpp"
#include "track.hpp"


namespace Dino {


  Track::SequenceIterator::SequenceIterator() : m_vector(NULL) {

  }
  
  
  const Track::SequenceEntry* Track::SequenceIterator::operator->() const {
    return *m_iter;
  }
  
  
  const Track::SequenceEntry& Track::SequenceIterator::operator*() const {
    return **m_iter;
  }
  
  
  bool Track::SequenceIterator::operator==(const SequenceIterator& iter) const {
    return (m_iter == iter.m_iter);
  }
  
  
  bool Track::SequenceIterator::operator!=(const SequenceIterator& iter) const {
    return (m_iter != iter.m_iter);
  }
  
  
  Track::SequenceIterator& Track::SequenceIterator::operator++() {
    const SequenceEntry* old_ptr = *m_iter;
    do {
      ++m_iter;
    } while (m_iter != m_vector->end() && 
	     (*m_iter == old_ptr || *m_iter == NULL));
    return *this;
  }
  

  Track::SequenceIterator::SequenceIterator(const 
					    std::vector<SequenceEntry*>::
					    const_iterator& 
					    iter,
					    const std::vector<SequenceEntry*>&
					    vec) 
    : m_iter(iter),
      m_vector(&vec) {
    
  }


  Track::ConstPatternIterator::ConstPatternIterator() {

  }
  
  
  const Pattern* Track::ConstPatternIterator::operator->() const {
    return m_iter->second;
  }
  
  
  const Pattern& Track::ConstPatternIterator::operator*() const {
    return *m_iter->second;
  }

  
  bool Track::ConstPatternIterator::operator==(const ConstPatternIterator& iter)
    const {
    return (m_iter == iter.m_iter);
  }
  
  
  bool Track::ConstPatternIterator::operator!=(const ConstPatternIterator& iter)
    const {
    return (m_iter != iter.m_iter);
  }
  
  
  Track::ConstPatternIterator& Track::ConstPatternIterator::operator++() {
    ++m_iter;
    return *this;
  }
  
  
  Track::ConstPatternIterator::ConstPatternIterator(const PatternIterator& iter)
    : m_iter(iter.m_iter) {

  }
  
  
  Track::ConstPatternIterator::
  ConstPatternIterator(const std::map<int, Pattern*>::const_iterator& iter)
    : m_iter(iter) {

  }


  Track::PatternIterator::PatternIterator() {

  }
  
  
  const Pattern* Track::PatternIterator::operator->() const {
    return m_iter->second;
  }
  
  
  const Pattern& Track::PatternIterator::operator*() const {
    return *m_iter->second;
  }
  
  
  Pattern* Track::PatternIterator::operator->() {
    return m_iter->second;
  }
  
  
  Pattern& Track::PatternIterator::operator*() {
    return *m_iter->second;
  }
  
  
  bool Track::PatternIterator::operator==(const PatternIterator& iter) const {
    return (m_iter == iter.m_iter);
  }
  
  
  bool Track::PatternIterator::operator!=(const PatternIterator& iter) const {
    return (m_iter != iter.m_iter);
  }
  
  
  Track::PatternIterator& Track::PatternIterator::operator++() {
    ++m_iter;
    return *this;
  }
      

  Track::PatternIterator::
  PatternIterator(const std::map<int, Pattern*>::iterator& iter)
    : m_iter(iter) {

  }


  Track::Track(int id, int length, const string& name) 
    : m_id(id),
      m_name(name),
      m_sequence(new vector<SequenceEntry*>(length, (SequenceEntry*)NULL)),
      m_dirty(false) {
  
    dbg1<<"Creating track \""<<name<<"\""<<endl;
  }


  Track::~Track() {
    dbg1<<"Destroying track \""<<m_name<<"\""<<endl;
    map<int, Pattern*>::iterator iter;
    for (iter = m_patterns.begin(); iter != m_patterns.end(); ++iter)
      delete iter->second;
    SequenceEntry* ptr = NULL;
    for (unsigned int i = 0; i < m_sequence->size(); ++i) {
      if ((*m_sequence)[i] != ptr)
	delete (*m_sequence)[i];
      ptr = (*m_sequence)[i];
    }
    delete m_sequence;
  }


  int Track::get_id() const {
    return m_id;
  }
  

  /** Returns the name of the track. The name is just a label that the user
      can change. */
  const string& Track::get_name() const {
    return m_name;
  }

  
  /** Returns a map of the patterns in this track. This map can be changed
      by the user, and we have no way of knowing how it's changed - this will
      probably be replaced by higher level functions in the future. */
  /*
  map<int, Pattern*>& Track::get_patterns() {
    return m_patterns;
  }


  const map<int, Pattern*>& Track::get_patterns() const {
    return m_patterns;
  }


  Pattern* Track::get_pattern(int id) {
    map<int, Pattern*>::iterator iter = m_patterns.find(id);
    if (iter != m_patterns.end())
      return iter->second;
    return NULL;
  }
  */
  

  Track::ConstPatternIterator Track::pat_begin() const {
    return ConstPatternIterator(m_patterns.begin());
  }
  
  
  Track::ConstPatternIterator Track::pat_end() const {
    return ConstPatternIterator(m_patterns.end());
  }
  
  
  Track::ConstPatternIterator Track::pat_find(int id) const {
    return ConstPatternIterator(m_patterns.find(id));
  }


  Track::PatternIterator Track::pat_begin() {
    return PatternIterator(m_patterns.begin());
  }
  
  
  Track::PatternIterator Track::pat_end() {
    return PatternIterator(m_patterns.end());
  }
  
  
  Track::PatternIterator Track::pat_find(int id) {
    return PatternIterator(m_patterns.find(id));
  }


  Track::SequenceIterator Track::seq_begin() const {
    std::vector<SequenceEntry*>::const_iterator iter;
    for (iter = m_sequence->begin(); iter != m_sequence->end(); ++iter) {
      if (*iter != NULL)
	break;
    }
    return SequenceIterator(iter, *m_sequence);
  }
  
  
  Track::SequenceIterator Track::seq_end() const {
    return SequenceIterator(m_sequence->end(), *m_sequence);
  }
  
  
  Track::SequenceIterator Track::seq_find(unsigned int beat) const {
    if ((*m_sequence)[beat] != NULL)
      return SequenceIterator(m_sequence->begin() + beat, *m_sequence);
    return SequenceIterator(m_sequence->end(), *m_sequence);
  }

  
  /** Returns the MIDI channel for this track. */
  int Track::get_channel() const {
    return m_channel;
  }


  unsigned int Track::get_length() const {
    return m_sequence->size();
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
  Track::PatternIterator Track::add_pattern(const string& name, int length, 
					    int steps) {
    /* This does not actually need to be threadsafe since the sequencer
       never accesses the patterns through the map, only through the 
       sequence entries. */
    int id;
    if (m_patterns.rbegin() != m_patterns.rend())
      id = m_patterns.rbegin()->first + 1;
    else
      id = 1;
    m_patterns[id] = new Pattern(id, name, length, steps);
    signal_pattern_added(id);
    return PatternIterator(m_patterns.find(id));
  }

  
  /** Removes the pattern with the given ID. */
  void Track::remove_pattern(int id) {
    /* Changing the map itself does not need to be threadsafe since the 
       sequencer never accesses patterns through the map, but actually
       deleting the pattern must be done in a threadsafe way. */
    map<int, Pattern*>::iterator iter = m_patterns.find(id);
    if (iter != m_patterns.end()) {
      
      for (unsigned int i = 0; i < m_sequence->size(); ++i) {
	if ((*m_sequence)[i] != 0 && (*m_sequence)[i]->pattern == iter->second)
	  remove_sequence_entry(seq_find(i));
      }
      
      Deleter::queue(iter->second);
      m_patterns.erase(iter);
      signal_pattern_removed(id);
    }
  }


  /** Set the sequency entry at the given beat to the given pattern and length.*/
  Track::SequenceIterator Track::set_sequence_entry(int beat, 
						    int pattern, int length) {
    assert(beat >= 0);
    assert(beat < int(m_sequence->size()));
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
    SequenceEntry* se = (*m_sequence)[beat];
    if (se) {
      int stop = se->start + se->length;
      for (int i = stop - 1; i >= beat; --i)
	(*m_sequence)[i] = NULL;
      if (se->start == unsigned(beat))
	Deleter::queue(se);
      else
	se->length = beat - se->start;
    }
  
    // make sure the new sequence entry fits
    for (int i = beat; i < beat + newLength; ++i) {
      if (i >= int(m_sequence->size()) || (*m_sequence)[i]) {
	newLength = i - beat;
	break;
      }
    }
  
    se  = new SequenceEntry(pattern, m_patterns.find(pattern)->second, 
			    beat, newLength);
    int i;
    for (i = beat; i < beat + newLength; ++i)
      (*m_sequence)[i] = se;
    
    signal_sequence_entry_added(beat, (*m_sequence)[beat]->pattern->get_id(), 
				newLength);
    return SequenceIterator(m_sequence->begin() + i, *m_sequence);
  }


  void Track::set_seq_entry_length(SequenceIterator iter, unsigned int length) {
    unsigned beat = iter->start;
    assert((*m_sequence)[beat]);
    SequenceEntry* se = (*m_sequence)[beat];
    if (length == se->length)
      return;
    else if (length > se->length) {
      unsigned int i;
      for (i = se->start + se->length; i < se->start + length; ++i) {
	if ((*m_sequence)[i])
	  break;
	(*m_sequence)[i] = se;
      }
      se->length = i - se->start;
    }
    else {
      int tmp = se->length;
      se->length = length;
      for (int i = se->start + tmp - 1; i >= int(se->start + se->length); --i)
	(*m_sequence)[i] = NULL;
    }
    signal_sequence_entry_changed(beat, se->pattern->get_id(), se->length);
  }



  /** Remove the sequence entry (pattern) that is playing at the given beat.
      If no pattern is playing at that beat, return @c false. */
  bool Track::remove_sequence_entry(SequenceIterator iterator) {
    unsigned beat = iterator->start;
    SequenceEntry* se = (*m_sequence)[beat];
    if (se) {
      int start = se->start;
      for (int i = se->start + se->length - 1; i >= int(se->start); --i)
	(*m_sequence)[i] = NULL;
      Deleter::queue(se);
      signal_sequence_entry_removed(start);
      return true;
    }
    return false;
  }


  /** Set the length of the track. Only the Song should do this. */
  void Track::set_length(int length) {
    
    assert(length > 0);
    if (length != int(m_sequence->size())) {
      for (unsigned i = length; i < m_sequence->size(); ++i) {
	if ((*m_sequence)[i]) {
	  if ((*m_sequence)[i]->start < (unsigned)length) {
	    set_seq_entry_length(seq_find((*m_sequence)[i]->start), 
				 length - (*m_sequence)[i]->start);
	  }
	  else {
	    remove_sequence_entry(seq_find(i));
	  }
	}
      }
      
      vector<SequenceEntry*>* new_seq = new vector<SequenceEntry*>(*m_sequence);
      new_seq->resize(length);
      vector<SequenceEntry*>* old_seq = m_sequence;
      m_sequence = new_seq;
      Deleter::queue(old_seq);
      
      signal_length_changed(m_sequence->size());
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
    for (unsigned int i = 0; i < m_sequence->size(); ++i) {
      SequenceEntry* se = (*m_sequence)[i];
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
      int id, length, steps;
      sscanf(pat_elt->get_attribute("id")->get_value().c_str(), "%d", &id);
      sscanf(pat_elt->get_attribute("length")->get_value().c_str(), 
	     "%d", &length);
      sscanf(pat_elt->get_attribute("steps")->get_value().c_str(), 
	     "%d", &steps);
      string name = pat_elt->get_attribute("name")->get_value();
      m_patterns[id] = new Pattern(id, name, length, steps);
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


  int Track::get_events(double beat, double before_beat,
			const MIDIEvent** events, double* beats, int& room,
			const InterpolatedEvent** ip_events, double* ip_beats, 
			int& ip_room) const {
    assert(beat >= 0);
    assert(before_beat >= beat);

    const vector<SequenceEntry*>& sequence = *m_sequence;
    if (beat >= sequence.size())
      return 0;
    if (before_beat > sequence.size())
      before_beat = sequence.size();
    
    // iterate over all beats that intersects the interval [beat, before_beat)
    int event_start = 0;
    for (unsigned i = unsigned(beat); i < before_beat; ++i) {
      if (sequence[i] != NULL) {
	
	// compute start in pattern time
	double start = beat - sequence[i]->start;
	if (start < 0)
	  start = 0;
	
	// compute end in pattern time
	double end = before_beat - sequence[i]->start;
	if (end > sequence[i]->length)
	  end = sequence[i]->length;
	
	// retrieve the pattern events
	int n = sequence[i]->pattern->
	  get_events(beat - sequence[i]->start, end, events + event_start, 
		     beats + event_start, room, ip_events, ip_beats, ip_room);
	
	// convert timestamps to song time
	for (int j = event_start; j < event_start + n; ++j)
	  beats[j] += sequence[i]->start;
	event_start += n;
	
	// move forward to the end of this sequence entry
	i = sequence[i]->start + sequence[i]->length - 1;
	
	// stop any notes playing in this sequence entry
	// XXX This should probably be done in the pattern instead since we
	// don't know if there are any events after end - 0.001
	if (i + 1 < before_beat && event_start < room) {
	  events[event_start] = &MIDIEvent::AllNotesOff;
	  beats[event_start] = i + 1 - 0.001;
	  ++event_start;
	}
      }
    }
    
    room -= event_start;
    return event_start;
  }


}
