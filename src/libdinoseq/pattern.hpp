#ifndef PATTERN_H
#define PATTERN_H

#include <list>
#include <map>
#include <vector>


#include <sigc++/signal.h>
#include <libxml++/libxml++.h>

#include "controller.hpp"
#include "deletable.hpp"
#include "note.hpp"
#include "noteevent.hpp"
#include "xmlserialisable.hpp"


using namespace Glib;
using namespace sigc;
using namespace std;
using namespace xmlpp;


namespace Dino {


  /** This class stores information about a pattern. A pattern is a sequence of
      notes and MIDI control changes which can be played at a certain time
      by the sequencer. */
  class Pattern : public Deletable, public XMLSerialisable {
  public:
    
    /** This class is used to access the note data in a Pattern.
	@b Example:
	@code
	int count_notes(const Dino::Pattern& pat) {
	  Dino::Pattern::NoteIterator iter;
	  int n = 0;
	  for (iter = pat.notes_begin(); iter != pat.notes_end(); ++iter, ++n);
	  return n;
	}
	@endcode
	
	@see Pattern::notes_begin(), Pattern::notes_end(), Pattern::find_note()
    */
    class NoteIterator {
    public:
      
      /** Create an invalid iterator. */
      NoteIterator();
      
      /** Dereference the iterator to get a constant Note pointer. */
      const Note* operator*() const;
      /** Dereference the iterator to get a constant Note pointer. */
      const Note* operator->() const;
      /** Returns @c true if the two iterators refer to the same note. */
      bool operator==(const NoteIterator& iter) const;
      /** Returns @c true if the two iterators does not refer to the 
	  same note. */
      bool operator!=(const NoteIterator& iter) const;
      /** Advances the iterator to the next note. */
      NoteIterator& operator++();
      /** If the iterator is definitely invalid, this will return @c false.
	  However, it might return @c true for an iterator that has been 
	  invalidated by removing the note it refers to, for example. */
      operator bool() const {
	return (m_pattern != NULL && m_note != NULL);
      }
	  
    private:
      
      friend class Pattern;
      
      NoteIterator(const Pattern* pat, Note* event);
      
      const Pattern* m_pattern;
      Note* m_note;
    };
    
    
    Pattern(const string& name, int length, int steps);
  
    ~Pattern();
  
    typedef vector<NoteEvent*> NoteEventList;
  
    // accessors
    /** Return the name of this pattern. */
    const string& get_name() const;
    /** Return an iterator that refers to the first note in the pattern. */
    NoteIterator notes_begin() const;
    /** Return an invalid iterator that can be used to check when an iterator
	has passed the last note in this pattern. */
    NoteIterator notes_end() const;
    /** Return the number of steps per beat. */
    int get_steps() const;
    /** Return the length in beats. */
    int get_length() const;
    /** Return the bounding rectangle in the (step, key) plane for all notes
	that have been changed since the last call to reset_dirty_rect(). */
    void get_dirty_rect(int* min_step, int* min_note, 
			int* max_step, int* max_note) const;
    const std::vector<Controller>& get_controllers() const;
    /** Return an iterator for the note with key @c value that is playing at
	step @c step, or an invalid iterator if there is no such note. */
    NoteIterator find_note(unsigned int step, int value) const;
    
    // mutators
    void set_name(const string& name);
    void add_note(unsigned step, int value, int velocity, int length);
    void delete_note(NoteIterator note);
    int resize_note(NoteIterator note, int length);
    void set_velocity(NoteIterator note, unsigned char velocity);
    void add_cc(unsigned int controller, unsigned int step, 
		unsigned char value);
    void remove_cc(unsigned int controller, unsigned int step);
    void reset_dirty_rect();
    
    // XML I/O
    bool is_dirty() const;
    void make_clean() const;
    bool fill_xml_node(Element* elt) const;
    bool parse_xml_node(const Element* elt);
    
    // sequencing
    MIDIEvent* get_events(unsigned int& beat, unsigned int& tick, 
			  unsigned int before_beat, 
			  unsigned int before_tick,
			  unsigned int ticks_per_beat, 
			  unsigned int& list) const;
    
    int get_events2(unsigned int& beat, unsigned int& tick,
		    unsigned int before_beat, unsigned int before_tick,
		    unsigned int ticks_per_beat, 
		    MIDIEvent** events, int room) const;
  public:
  
    sigc::signal<void, string> signal_name_changed;
    sigc::signal<void, int> signal_length_changed;
    sigc::signal<void, int> signal_steps_changed;
    sigc::signal<void, int, int, int> signal_note_added;
    sigc::signal<void, int, int, int> signal_note_changed;
    sigc::signal<void, int, int> signal_note_removed;
    sigc::signal<void, int, int, int> signal_cc_added;
    sigc::signal<void, int, int, int> signal_cc_changed;
    sigc::signal<void, int, int> signal_cc_removed;
    sigc::signal<void, int> signal_controller_added;
    sigc::signal<void, int> signal_controller_removed;

  private:
    
    //friend class Pattern::NoteIterator;
    
    // no copying for now
    Pattern(const Pattern&) { }
    Pattern& operator=(const Pattern&) { return *this; }
    
    NoteIterator find_note_on(unsigned start, unsigned end, unsigned char key);
    NoteIterator find_note_on_before(unsigned step);
    
    
    /** The name of the pattern */
    string m_name;
    /** Pattern length in beats */
    unsigned int m_length;
    /** Number of steps per beat */
    unsigned int m_steps;
    /** The Note objects. */
    //Note* m_notes;
    /** The note on events in the pattern */
    NoteEventList m_note_ons;
    /** The note off events in the pattern */
    NoteEventList m_note_offs;
    /** The control change event lists */
    std::vector<Controller> m_controllers;
    
    mutable bool m_dirty;
  
    mutable unsigned long m_last_beat;
    mutable unsigned long m_last_tick;
    mutable bool m_already_returned;
  
    // dirty rect
    int m_min_step, m_min_note, m_max_step, m_max_note;
  
  };


}


#endif
