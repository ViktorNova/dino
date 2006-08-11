/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#ifndef PATTERN_HPP
#define PATTERN_HPP

#include <cassert>
#include <iterator>
#include <list>
#include <map>
#include <vector>


#include <sigc++/signal.h>
#include <libxml++/libxml++.h>

#include "xmlserialisable.hpp"


using namespace Glib;
using namespace sigc;
using namespace std;
using namespace xmlpp;


namespace Dino {
  
  
  class Controller;
  class InterpolatedEvent;
  class MIDIBuffer;
  class Note;
  class NoteCollection;
  class NoteEvent;
  class PatternSelection;
  

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
    class NoteIterator : public std::iterator<std::forward_iterator_tag, Note> {
    public:
      
      /** Create an invalid iterator. */
      NoteIterator();
      
      /** Dereference the iterator to get a constant Note reference. */
      Note& operator*() const;
      /** Dereference the iterator to get a constant Note pointer. */
      Note* operator->() const;
      /** Returns @c true if the two iterators refer to the same note. */
      bool operator==(const NoteIterator& iter) const;
      /** Returns @c true if the two iterators does not refer to the 
          same note. */
      bool operator!=(const NoteIterator& iter) const;
      /** Returns @c true if the (step, key) pair for the first iterator
          is smaller than the pair for the second iterator. Needed for storing
          this class in std::sets. */
      bool operator<(const NoteIterator& iter) const;
      /** Advances the iterator to the next note. */
      NoteIterator& operator++();
      NoteIterator operator++(int);
    
    private:
      
      friend class Pattern;
      
      NoteIterator(const Pattern* pat, Note* event);
      
      const Pattern* m_pattern;
      Note* m_note;
    };
    
    
    /** A ControllerIterator is a const_iterator type that can be used to
        access data from controllers in the pattern. */
    class ControllerIterator : 
      public std::iterator<std::forward_iterator_tag, Controller> {
    public:
      
      /** Create an invalid iterator. */
      ControllerIterator() { }
      
      /** Dereference the iterator to get a constant Controller reference. */
      const Controller& operator*() const { return **m_iterator; }
      /** Dereference the iterator to get a constant Controller pointer. */
      const Controller* operator->() const { return *m_iterator; }
      /** Returns @c true if the two iterators refer to the same Controller. */
      bool operator==(const ControllerIterator& iter) const {
        return (m_iterator == iter.m_iterator);
      }
      /** Returns @c false if the two iterators refer to the same Controller. */
      bool operator!=(const ControllerIterator& iter) const {
        return (m_iterator != iter.m_iterator);
      }
      /** Advances the iterator to the next controller. */
      ControllerIterator& operator++() { ++m_iterator; return *this; }
      
    private:
      
      friend class Pattern;

      ControllerIterator(const std::vector<Controller*>::iterator& iter) 
        : m_iterator(iter) { 
      }
      
      std::vector<Controller*>::iterator m_iterator;
    };

    /** Create a new pattern. */
    Pattern(int id, const string& name, int length, int steps);
    /** Create a new pattern as a copy of @c pat. */
    Pattern(int id, const Pattern& pat);
    
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
    ControllerIterator ctrls_find(long param) const;
    /** Return the number of steps per beat. */
    unsigned int get_steps() const;
    /** Return the length in beats. */
    unsigned int get_length() const;
    /** Return the bounding rectangle in the (step, key) plane for all notes
        that have been changed since the last call to reset_dirty_rect(). */
    void get_dirty_rect(int* min_step, int* min_note, 
                        int* max_step, int* max_note) const;
    //@}
    
    /// @name Mutators
    //@{
    /** Set the name of this pattern. */
    void set_name(const string& name);
    /** Change the length in beats. */
    void set_length(unsigned int length);
    /** Change the number of steps per beat. */
    void set_steps(unsigned int steps);
    /** Add a note at the given step with the given key, velocity, and length
        (in steps). */
    NoteIterator add_note(unsigned step, int key, int velocity, int length);
    /** Add a collection of notes with the given step and key offsets. 
        Can be used as a "paste" command. */
    void add_notes(const NoteCollection& notes, unsigned step, int key,
                   PatternSelection* selection = 0);
    /** Delete a note. */
    void delete_note(NoteIterator note);
    /** Change the length of a note. */
    int resize_note(NoteIterator note, int length);
    /** Set the velocity of a note. */
    void set_velocity(NoteIterator note, unsigned char velocity);
    /** Add a controller for this pattern. */
    ControllerIterator add_controller(const std::string& name, long param, 
                                      int min, int max);
    /** Remove a controller. */
    void remove_controller(ControllerIterator iter);
    /** Add a CC event to the given controller. */
    void add_cc(ControllerIterator iter, unsigned int step, int value);
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
    void sequence(MIDIBuffer& buffer, double from, double to, double offset, 
                  unsigned int pattern_length, int channel) const;
    //@}
    
  public:
    
    /// @name Signals
    //@{
    /** Emitted when the pattern name has changed. */
    sigc::signal<void, string>& signal_name_changed();
    /** Emitted when the length in beats has changed. */
    sigc::signal<void, int>& signal_length_changed();
    /** Emitted when the number of steps per beat has changed. */
    sigc::signal<void, int>& signal_steps_changed();
    /** Emitted when a note has been added. */
    sigc::signal<void, Note const&>& signal_note_added();
    /** Emitted when an existing note has been changed. */
    sigc::signal<void, Note const&>& signal_note_changed();
    /** Emitted when a note has been removed. */
    sigc::signal<void, Note const&>& signal_note_removed();
    /** Emitted when a CC control point has been added. */
    sigc::signal<void, int, int, int>& signal_cc_added();
    /** Emitted when the value for a CC control point has changed. */
    sigc::signal<void, int, int, int>& signal_cc_changed();
    /** Emitted when a CC control point has been removed. */
    sigc::signal<void, int, int>& signal_cc_removed();
    /** Emitted when a whole controller has been added. */
    sigc::signal<void, int>& signal_controller_added();
    /** Emitted when a whole controller has been removed. */
    sigc::signal<void, int>& signal_controller_removed();
    //@}
    
  private:
    
    typedef vector<NoteEvent*> NoteEventList;
    
    /** This struct is used internally so we can swap all data used by the
        sequencer with a single pointer assignment (for lock-free 
        thread safety). */
    struct SeqData {
      SeqData(NoteEventList* note_ons, NoteEventList* note_offs, 
              std::vector<Controller*>* controllers,
              unsigned int l, unsigned int s);
      ~SeqData();
      
      NoteEventList* ons;
      NoteEventList* offs;
      std::vector<Controller*>* ctrls;
      unsigned int length;
      unsigned int steps;
    };
    
    // no copying for now
    Pattern(const Pattern&) { assert(0); }
    Pattern& operator=(const Pattern&) { assert(0); return *this; }
    
    /** Find a note on event with the specified @c key between the steps
        @c start and @c end. */
    NoteIterator find_note_on(unsigned start, unsigned end, unsigned char key);
    /** Delete a note in a safe way. */
    void delete_note(Note* note);
    /** Resize a note. */
    int resize_note(Note* note, int length);
    
    /** The pattern id. */
    int m_id;
    /** The name of the pattern */
    string m_name;
    /** The data used by the sequencing functions need to be stored in a
        single structure so we can modify it in a lock-free way by swapping 
        a single pointer. */
    SeqData* volatile m_sd;
    
    mutable bool m_dirty;
  
    // dirty rect
    int m_min_step, m_min_note, m_max_step, m_max_note;

    sigc::signal<void, string> m_signal_name_changed;
    sigc::signal<void, int> m_signal_length_changed;
    sigc::signal<void, int> m_signal_steps_changed;
    sigc::signal<void, Note const&> m_signal_note_added;
    sigc::signal<void, Note const&> m_signal_note_changed;
    sigc::signal<void, Note const&> m_signal_note_removed;
    sigc::signal<void, int, int, int> m_signal_cc_added;
    sigc::signal<void, int, int, int> m_signal_cc_changed;
    sigc::signal<void, int, int> m_signal_cc_removed;
    sigc::signal<void, int> m_signal_controller_added;
    sigc::signal<void, int> m_signal_controller_removed;

  };


}


#endif
