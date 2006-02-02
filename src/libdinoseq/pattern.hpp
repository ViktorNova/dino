#ifndef PATTERN_H
#define PATTERN_H

#include <list>
#include <map>
#include <vector>


#include <sigc++/signal.h>
#include <libxml++/libxml++.h>

#include "controller.hpp"
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
  class Pattern : public XMLSerialisable {
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
    
    
    /** A ControllerIterator is a const_iterator type that can be used to
	access data from controllers in the pattern. */
    class ControllerIterator {
    public:
      
      ControllerIterator() { }
      
      const Controller& operator*() const { return **m_iterator; }
      const Controller* operator->() const { return *m_iterator; }
      bool operator==(const ControllerIterator& iter) const {
	return (m_iterator == iter.m_iterator);
      }
      bool operator!=(const ControllerIterator& iter) const {
	return (m_iterator != iter.m_iterator);
      }
      ControllerIterator& operator++() { ++m_iterator; return *this; }
      
    private:
      
      friend class Pattern;

      ControllerIterator(const std::vector<Controller*>::iterator& iter) 
	: m_iterator(iter) { 
      }
      
      std::vector<Controller*>::iterator m_iterator;
    };

    
    Pattern(int id, const string& name, int length, int steps);
  
    ~Pattern();
  
    /// @name Accessors
    //@{
    int get_id() const;
    /** Return the name of this pattern. */
    const string& get_name() const;
    /** Return an iterator that refers to the first note in the pattern. */
    NoteIterator notes_begin() const;
    /** Return an invalid iterator that can be used to check when an iterator
	has passed the last note in this pattern. */
    NoteIterator notes_end() const;
    /** Return an iterator for the note with key @c value that is playing at
	step @c step, or an invalid iterator if there is no such note. */
    NoteIterator find_note(unsigned int step, int value) const;
    /** Return an iterator that refers to the first controller in the pattern.*/
    ControllerIterator ctrls_begin() const;
    /** Return an invalid iterator that can be used to check when an iterator
	has passed the last controller in the pattern. */
    ControllerIterator ctrls_end() const;
    /** Return an iterator for the controller with parameter @c param, or an
	invalid iterator if no such controller exists. */
    ControllerIterator ctrls_find(unsigned long param) const;
    /** Return the number of steps per beat. */
    int get_steps() const;
    /** Return the length in beats. */
    int get_length() const;
    /** Return the bounding rectangle in the (step, key) plane for all notes
	that have been changed since the last call to reset_dirty_rect(). */
    void get_dirty_rect(int* min_step, int* min_note, 
			int* max_step, int* max_note) const;
    //@}
    
    /// @name Mutators
    //@{
    /** Set the name of this pattern. */
    void set_name(const string& name);
    /** Add a note at the given step with the given key, velocity, and length
	(in steps). */
    void add_note(unsigned step, int key, int velocity, int length);
    /** Delete a note. */
    void delete_note(NoteIterator note);
    /** Change the length of a note. */
    int resize_note(NoteIterator note, int length);
    /** Set the velocity of a note. */
    void set_velocity(NoteIterator note, unsigned char velocity);
    /** Add a controller for this pattern. */
    ControllerIterator add_controller(unsigned long param, int min, int max);
    /** Remove a controller. */
    void remove_controller(ControllerIterator iter);
    /** Add a CC event to the given controller. */
    void add_cc(ControllerIterator iter, unsigned int step,unsigned char value);
    /** Remove a CC event. */
    void remove_cc(ControllerIterator iter, unsigned int step);
    /** Reset the "dirty rect".
	@see get_dirty_rect(). */
    void reset_dirty_rect();
    //@}
    
    /// @name XML I/O
    //@{
    bool is_dirty() const;
    void make_clean() const;
    bool fill_xml_node(Element* elt) const;
    bool parse_xml_node(const Element* elt);
    //@}
    
    /// @name Sequencing
    //@{
    int get_events(double beat, double before_beat,
		   const MIDIEvent** events, double* beats, int room) const;
    //@}
    
  public:
    
    /// @name Signals
    //@{
    /** Emitted when the pattern name has changed. */
    sigc::signal<void, string> signal_name_changed;
    /** Emitted when the length in beats has changed. */
    sigc::signal<void, int> signal_length_changed;
    /** Emitted when the number of steps per beat has changed. */
    sigc::signal<void, int> signal_steps_changed;
    /** Emitted when a note has been added. */
    sigc::signal<void, int, int, int> signal_note_added;
    /** Emitted when an existing note has been changed. */
    sigc::signal<void, int, int, int> signal_note_changed;
    /** Emitted when a note has been removed. */
    sigc::signal<void, int, int> signal_note_removed;
    /** Emitted when a CC control point has been added. */
    sigc::signal<void, int, int, int> signal_cc_added;
    /** Emitted when the value for a CC control point has changed. */
    sigc::signal<void, int, int, int> signal_cc_changed;
    /** Emitted when a CC control point has been removed. */
    sigc::signal<void, int, int> signal_cc_removed;
    /** Emitted when a whole controller has been added. */
    sigc::signal<void, int> signal_controller_added;
    /** Emitted when a whole controller has been removed. */
    sigc::signal<void, int> signal_controller_removed;
    //@}
    
  private:
    
    typedef vector<NoteEvent*> NoteEventList;
  
    // no copying for now
    Pattern(const Pattern&) { }
    Pattern& operator=(const Pattern&) { return *this; }
    
    NoteIterator find_note_on(unsigned start, unsigned end, unsigned char key);
    
    int m_id;
    /** The name of the pattern */
    string m_name;
    /** Pattern length in beats */
    unsigned int m_length;
    /** Number of steps per beat */
    unsigned int m_steps;
    /** The note on events in the pattern */
    NoteEventList m_note_ons;
    /** The note off events in the pattern */
    NoteEventList m_note_offs;
    /** The control change event lists */
    std::vector<Controller*>* m_controllers;
    
    mutable bool m_dirty;
  
    mutable unsigned long m_last_beat;
    mutable unsigned long m_last_tick;
    mutable bool m_already_returned;
  
    // dirty rect
    int m_min_step, m_min_note, m_max_step, m_max_note;
  
  };


}


#endif
