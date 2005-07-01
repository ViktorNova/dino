#ifndef TRACK_HPP
#define TRACK_HPP

#include <string>
#include <vector>

#include <glibmm/thread.h>
#include <sigc++/signal.h>
#include <libxml++/libxml++.h>


class MIDIEvent;
class Pattern;


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
    SequenceEntry(int patID, Pattern* patPtr, 
		  unsigned int st, unsigned int len) 
      : pattern_id(patID), pattern(patPtr), start(st), length(len) { }
    int pattern_id;
    Pattern* pattern;
    unsigned int start;
    unsigned int length;
  };
  
  
  Track(int length = 0, const string& name = "Untitled");
  
  // accessors
  const string& get_name() const;
  map<int, Pattern*>& get_patterns();
  const map<int, Pattern*>& get_patterns() const;
  //const vector<SequenceEntry*>& get_sequence() const;
  const SequenceEntry* get_seq_entry(unsigned int beat);
  int get_channel() const;
  unsigned int get_length() const;
  
  // mutators
  void set_name(const string& name);
  int add_pattern(const string& name, int length, int steps, int ccSteps);
  void set_sequence_entry(int beat, int pattern, int length = -1);
  void set_seq_entry_length(unsigned int beat, unsigned int length);
  bool remove_sequence_entry(int beat);
  void set_length(int length);
  void set_channel(int channel);
  
  // XML I/O
  bool is_dirty() const;
  void make_clean() const;
  bool fill_xml_node(Element* elt) const;
  bool parse_xml_node(const Element* elt);
  
  // sequencing
  MIDIEvent* get_events(unsigned int& beat, unsigned int& tick, 
		    unsigned int before_beat, unsigned int before_tick, 
		    unsigned int ticks_per_beat) const;
  
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
  int m_length;
  int m_channel;
  vector<SequenceEntry*> m_sequence;
  
  mutable bool m_dirty;
  
  mutable Mutex m_lock;
  
};


#endif
