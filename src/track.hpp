#ifndef TRACK_HPP
#define TRACK_HPP

#include <string>
#include <vector>

#include <glibmm/thread.h>
#include <sigc++/signal.h>
#include <libxml++/libxml++.h>

#include "pattern.hpp"


using namespace Glib;
using namespace sigc;
using namespace std;
using namespace xmlpp;


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
	prev(NULL), next(NULL) { }
    int pattern_id;
    Pattern* pattern;
    int start;
    int length;
    SequenceEntry* prev;
    SequenceEntry* next;
  };
  
  /** For convenience. */
  typedef SequenceEntry* Sequence;
  
  
  Track(int length = 0, const string& name = "Untitled");
  
  // accessors
  const string& get_name() const;
  map<int, Pattern*>& get_patterns();
  const map<int, Pattern*>& get_patterns() const;
  SequenceEntry* get_sequence();
  bool get_sequence_entry(int at, int& beat, int& pattern, int& length) const;
  int get_channel() const;
  
  // mutators
  void set_name(const string& name);
  int add_pattern(const string& name, int length, int steps, int ccSteps);
  void set_sequence_entry(int beat, int pattern, int length = -1);
  bool remove_sequence_entry(int beat);
  void set_length(int length);
  void set_channel(int channel);
  
  // XML I/O
  bool is_dirty() const;
  void make_clean() const;
  bool fill_xml_node(Element* elt) const;
  bool parse_xml_node(const Element* elt);
  
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
  map<int, Pattern*> m_patterns;
  SequenceEntry* m_seq_head;
  int m_length;
  int m_channel;
  
  mutable bool m_dirty;
  
  mutable Mutex m_lock;
  
  mutable SequenceEntry* m_current_seq_entry;
  
};


#endif
