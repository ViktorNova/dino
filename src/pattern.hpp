#ifndef PATTERN_H
#define PATTERN_H

#include <map>

#include <libxml/tree.h>


using namespace std;


/** This class stores information about a pattern. A pattern is a sequence of
    notes and MIDI control changes which can be played at a certain time
    by the sequencer. */
class Pattern {
public:
  
  Pattern();
  Pattern(int length, int steps, int cc_steps);
  
  struct CCEvent {
    CCEvent(int par, int val) 
      : param(par), value(val), next(NULL), previous(NULL) { }
    int param;
    int value;
    CCEvent* next;
    CCEvent* previous;
  };

  struct CCData {
    map<int, CCEvent*> changes;
  };
  
  /** Lexical comparison for pairs of ints */
  struct Compair {
    inline bool operator()(const pair<int,int>& p1, 
			   const pair<int, int>& p2) const{
      if (p1.first < p2.first)
	return true;
      else if (p1.first > p2.first)
	return false;
      return p1.second < p2.second;
    }
  };
  
  struct Note {
    Note(int stp, int val, int len) 
      : step(stp), value(val), length(len), next(NULL), previous(NULL) { }
    int step, value, length;
    Note* next;
    Note* previous;
  };
  
  typedef map<pair<int, int>, Note*, Compair> NoteMap;

  /** This function tries to add a new note with the given parameters. If 
      there is another note starting at the same step and with the same value,
      the old note will be deleted. If there is a note that is playing at that
      step with the same value, the old note will be shortened so it ends just
      before @c step. This function will also update the @c previous and 
      @c next pointers in the Note, so the doubly linked list will stay
      consistent with the note map. */
  void add_note(int step, int value, int length);
  /** This function will delete the note with the given value playing at the
      given step, if there is one. It will also update the @c previous and 
      @c next pointers so the doubly linked list will stay consistent with the
      note map. It will return the step that the deleted note started on,
      or -1 if no note was deleted. */
  int delete_note(int step, int value);
  void add_cc(int ccNumber, int step, int value);
  int delete_cc(int ccNumber, int step);
  
  NoteMap& get_notes();
  const NoteMap& get_notes() const;
  CCData& get_cc(int cc_number);
  int get_steps() const;
  int get_cc_steps() const;
  int get_length() const;
  void get_dirty_rect(int* min_step, int* min_note, 
		      int* max_step, int* max_note);
  
  /** This function will fill in the step, value, and length of the next note
      event that occurs before @c beforeStep, if there is one. If there isn't
      it will return false. It <b>must be realtime safe</b> because it is 
      used by the sequencer thread. */
  bool get_next_note(int& step, int& value, 
		     int& length, int before_step) const;
  /** This function sets "next note" to the first note at or after the given
      step. It <b>must be realtime safe</b> for @c step == 0 because that is
      used by the sequencer thread. */
  void find_next_note(int step) const;
  
  bool is_dirty() const;
  void make_clean() const;
  
  xmlNodePtr get_xml_node(xmlDocPtr doc) const;

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
