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

  typedef map<int, SequenceEntry*> Sequence;
  
  
  Track(int length = 0);
  
  /** Returns the name of the track. The name is just a label that the user
      can change. */
  const string& get_name() const;
  /** Returns a map of the patterns in this track. This map can be changed
      by the user, and we have no way of knowing how it's changed - this will
      probably be replaced by higher level functions in the future. */
  map<int, Pattern>& get_patterns();
  const map<int, Pattern>& get_patterns() const;
  /** Returns the sequence as a map of ints indexed by ints. This will probably
      be removed or made const in the future. */
  Sequence& get_sequence();
  /** Return the SequenceEntry that is playing at the given beat, or -1. */
  bool get_sequence_entry(int at, int& beat, int& pattern, int& length);
  /** Creates and adds a new pattern in this track with the given parameters.*/
  int add_pattern(int length, int steps, int ccSteps);
  
  /** Set the sequency entry at the given beat to the given pattern and length.
   */
  void set_sequence_entry(int beat, int pattern, int length = -1);
  /** Remove the sequence entry (pattern) that is playing at the given beat.
      If no pattern is playing at that beat, return @c false. */
  bool remove_sequence_entry(int beat);
  
  /** Set the length of the track. Only the Song should do this. */
  void set_length(int length);
  
  /** Has the track been changed since the last makeClean()? */
  bool is_dirty() const;
  /** Reset the dirty flag. */
  void make_clean() const;
  
  /** Get the next note event that occurs before the given time, or return 
      false if there isn't one. This function <b>must be realtime safe</b>
      because it is used by the sequencer thread. */
  bool get_next_note(int& beat, int& tick, int& value, int& length, 
		     int beforeBeat, int beforeTick) const;
  /** Get the next CC event, or return false if there isn't one. This 
      function <b>must be realtime safe</b> because it is used by the 
      sequencer thread. */
  bool get_next_cc_event(int& beat, int& tick, int& number, int& value) const;
  /** Sets "next note" to the next note after or at the given step. This 
      function does not have to be realtime safe. */  
  void find_next_note(int beat, int tick) const;
  
  /** Serialize this track to a XML node and return it. */
  xmlNodePtr get_xml_node(xmlDocPtr doc) const;
  
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
