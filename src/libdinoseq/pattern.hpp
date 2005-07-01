#ifndef PATTERN_H
#define PATTERN_H

#include <list>
#include <map>
#include <vector>


#include <glibmm/thread.h>
#include <sigc++/signal.h>
#include <libxml++/libxml++.h>


using namespace Glib;
using namespace sigc;
using namespace std;
using namespace xmlpp;


/** This class stores information about a pattern. A pattern is a sequence of
    notes and MIDI control changes which can be played at a certain time
    by the sequencer. */
class Pattern {
public:
  
  Pattern();
  Pattern(const string& name, int length, int steps, int cc_steps);
  
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
  
  /** This struct contains information about a Note on or Note off event. */
  struct NoteEvent {
    NoteEvent(bool on, int stp, int val, int vel, int len, 
	      NoteEvent* ass = NULL) 
      : note_on(on), step(stp), value(val), length(len), 
	velocity(vel), assoc(ass), previous(NULL), next(NULL) { }
    bool note_on;
    int step, value, length, velocity;
    NoteEvent* assoc;
    NoteEvent* previous;
    NoteEvent* next;
  };
  
  // accessors
  const string& get_name() const;
  const vector<NoteEvent*>& get_notes() const;
  CCData& get_cc(int cc_number);
  int get_steps() const;
  int get_cc_steps() const;
  int get_length() const;
  void get_dirty_rect(int* min_step, int* min_note, 
		      int* max_step, int* max_note);
  
  // mutators
  void set_name(const string& name);
  void add_note(int step, int value, int length);
  int delete_note(int step, int value);
  void add_cc(int ccNumber, int step, int value);
  int delete_cc(int ccNumber, int step);
  
  // XML I/O
  bool is_dirty() const;
  void make_clean() const;
  bool fill_xml_node(Element* elt) const;
  bool parse_xml_node(const Element* elt);
  
  NoteEvent* get_events(int beat, int tick, int before_beat, int before_tick,
			int ticks_per_beat) const;
  
public:
  
  signal<void, string> signal_name_changed;
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
  
  bool find_note_event(int step, int value, bool note_on, NoteEvent*& event);
  void delete_note_event(int step, NoteEvent* event);
  void add_note_event(int step, NoteEvent* event);
  
  string m_name;
  /** Pattern length in beats */
  int m_length;
  /** Number of steps per beat */
  int m_steps;
  /** The notes in the pattern */
  vector<NoteEvent*> m_notes;
  /** Number of CC steps per beat */
  int m_cc_steps;
  /** The MIDI control changes in the pattern */
  map<int, CCData> m_control_changes;
  
  mutable Mutex m_lock;
  
  mutable bool m_dirty;
  
  // dirty rect
  int m_min_step, m_min_note, m_max_step, m_max_note;
  
};


#endif
