#ifndef PATTERN_H
#define PATTERN_H

#include <map>

#include <sigc++/signal.h>
#include <libxml/tree.h>


using namespace sigc;
using namespace std;


/** This class stores information about a pattern. A pattern is a sequence of
    notes and MIDI control changes which can be played at a certain time
    by the sequencer. */
class Pattern {
public:
  
  Pattern();
  Pattern(int length, int steps, int cc_steps);
  
  /** This struct contains information about a single CC event. */
  struct CCEvent {
    CCEvent(int par, int val) 
      : param(par), value(val), next(NULL), previous(NULL) { }
    int param;
    int value;
    CCEvent* next;
    CCEvent* previous;
  };
  
  /** This struct contains information about the CC events for a certain
      CC number. */
  struct CCData {
    map<int, CCEvent*> changes;
  };
  
  /** Lexical comparison for pairs of ints */
  struct Compair {
    inline bool operator()(const pair<int, int>& p1, 
			   const pair<int, int>& p2) const{
      if (p1.first < p2.first)
	return true;
      else if (p1.first > p2.first)
	return false;
      return p1.second < p2.second;
    }
  };
  
  /** This struct contains information about a Note on / Note off pair. */
  struct Note {
    Note(int stp, int val, int len) 
      : step(stp), value(val), length(len), next(NULL), previous(NULL) { }
    int step, value, length;
    Note* next;
    Note* previous;
  };
  
  /** For convenience. */
  typedef map<pair<int, int>, Note*, Compair> NoteMap;
  
  // accessors
  NoteMap& get_notes();
  const NoteMap& get_notes() const;
  CCData& get_cc(int cc_number);
  int get_steps() const;
  int get_cc_steps() const;
  int get_length() const;
  void get_dirty_rect(int* min_step, int* min_note, 
		      int* max_step, int* max_note);
  
  // mutators
  void add_note(int step, int value, int length);
  int delete_note(int step, int value);
  void add_cc(int ccNumber, int step, int value);
  int delete_cc(int ccNumber, int step);
  
  // XML I/O
  bool is_dirty() const;
  void make_clean() const;
  xmlNodePtr get_xml_node(xmlDocPtr doc) const;
  
  // sequencing
  bool get_next_note(int& step, int& value, 
		     int& length, int before_step) const;
  void find_next_note(int step) const;
  
public:
  
  signal<void, int> signal_length_changed;
  signal<void, int> signal_steps_changed;
  signal<void, int> signal_cc_steps_changed;
  signal<void, int, int, int> signal_note_added;
  signal<void, int, int, int> signal_note_changed;
  signal<void, int, int> signal_note_removed;
  signal<void, int, int, int> signal_cc_added;
  signal<void, int, int, int> signal_cc_changed;
  signal<void, int, int> signal_cc_removed;

private:
  
  /** Pattern length in beats */
  int m_length;
  /** Number of steps per beat */
  int m_steps;
  /** The notes in the pattern */
  NoteMap m_notes;
  /** Number of CC steps per beat */
  int m_cc_steps;
  /** The MIDI control changes in the pattern */
  map<int, CCData> m_control_changes;
  
  mutable bool m_dirty;
  
  // dirty rect
  int m_min_step, m_min_note, m_max_step, m_max_note;
  
  mutable volatile Note* m_next_note;
  mutable volatile bool m_first_note;
};


#endif
