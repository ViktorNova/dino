#ifndef TRACK_HPP
#define TRACK_HPP

#include <string>
#include <vector>

#include <sigc++/signal.h>
#include <libxml/tree.h>

#include "pattern.hpp"


using namespace sigc;
using namespace std;


/** This class represents a track, which holds information about an instrument
    and how that instrument is played. It has a list of patterns, and a
    sequence that says when those patterns are played. 
*/
class Track {
public:
  
  /** This struct contains information about a sequence entry, i.e. a
      scheduled start and length for a pattern. */
  struct SequenceEntry {
    SequenceEntry(int patID, Pattern* patPtr, int st, int len) 
      : pattern_id(patID), pattern(patPtr), start(st), length(len), 
	next(NULL), previous(NULL) { }
    int pattern_id;
    Pattern* pattern;
    int start;
    int length;
    SequenceEntry* next;
    SequenceEntry* previous;
  };
  
  /** For convenience. */
  typedef map<int, SequenceEntry*> Sequence;
  
  
  Track(int length = 0);
  
  // accessors
  const string& get_name() const;
  map<int, Pattern>& get_patterns();
  const map<int, Pattern>& get_patterns() const;
  Sequence& get_sequence();
  bool get_sequence_entry(int at, int& beat, int& pattern, int& length) const;
  
  // mutators
  int add_pattern(int length, int steps, int ccSteps);
  void set_sequence_entry(int beat, int pattern, int length = -1);
  bool remove_sequence_entry(int beat);
  void set_length(int length);
  
  // XML I/O
  bool is_dirty() const;
  void make_clean() const;
  xmlNodePtr get_xml_node(xmlDocPtr doc) const;
  
  // sequencing
  bool get_next_note(int& beat, int& tick, int& value, int& length, 
		     int beforeBeat, int beforeTick) const;
  bool get_next_cc_event(int& beat, int& tick, int& number, int& value) const;
  void find_next_note(int beat, int tick) const;
  
  
public:

  signal<void, const string&> signal_name_changed;
  signal<void, int> signal_pattern_added; 
  signal<void, int> signal_pattern_removed;
  signal<void, int, int, int> signal_sequence_entry_added;
  signal<void, int, int, int> signal_sequence_entry_changed;
  signal<void, int> signal_sequence_entry_removed;
  signal<void, int> signal_length_changed;
  
private:
  
  string m_name;
  map<int, Pattern> m_patterns;
  Sequence m_sequence;
  int m_length;
  
  mutable bool m_dirty;
  
  mutable volatile SequenceEntry* m_current_seq_entry;
  mutable volatile Pattern::Note* m_next_note;
  
};


#endif
