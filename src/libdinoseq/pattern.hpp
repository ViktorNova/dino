/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
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

#include "eventlist.hpp"
#include "sequencable.hpp"
#include "songtime.hpp"
#include "xmlserialisable.hpp"


namespace Dino {
  
  
  class ControllerInfo;
  class Curve;
  class InterpolatedEvent;
  class MIDIBuffer;
  class Note;
  class NoteCollection;
  class NoteEvent;
  class NoteSelection;
  

  /** This class stores information about a pattern. A pattern is a sequence of
      notes and MIDI control changes which can be played at a certain time
      by the sequencer. */
  class Pattern : public XMLSerialisable, public Sequencable {
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
      
      /** Dereference the iterator to get a constant Event reference. */
      Event& operator*() const;
      /** Dereference the iterator to get a constant Event pointer. */
      Event* operator->() const;
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
      
      NoteIterator(const Pattern* pat, EventList<4, 4>::Node* node);
      
      const Pattern* m_pattern;
      EventList<4, 4>::Node* m_node;
    };
    
    
    /** A CurveIterator is a iterator type that can be used to
        access and modify data from curves in the pattern. */
    class CurveIterator : 
      public std::iterator<std::forward_iterator_tag, Curve> {
    public:
      
      /** Create an invalid iterator. */
      CurveIterator() { }
      
      /** Dereference the iterator to get a constant Controller reference. */
      Curve& operator*() { return **m_iterator; }
      /** Dereference the iterator to get a constant Controller pointer. */
      Curve* operator->() { return *m_iterator; }
      /** Dereference the iterator to get a constant Controller reference. */
      const Curve& operator*() const { return **m_iterator; }
      /** Dereference the iterator to get a constant Controller pointer. */
      const Curve* operator->() const { return *m_iterator; }
      /** Returns @c true if the two iterators refer to the same Curve. */
      bool operator==(const CurveIterator& iter) const {
        return (m_iterator == iter.m_iterator);
      }
      /** Returns @c false if the two iterators refer to the same Controller. */
      bool operator!=(const CurveIterator& iter) const {
        return (m_iterator != iter.m_iterator);
      }
      /** Advances the iterator to the next controller. */
      CurveIterator& operator++() { ++m_iterator; return *this; }
      
    private:
      
      friend class Pattern;

      CurveIterator(const std::vector<Curve*>::iterator& iter) 
        : m_iterator(iter) { 
      }
      
      std::vector<Curve*>::iterator m_iterator;
    };


    /** A CurveIterator is a const_iterator type that can be used to
        access data from curves in the pattern. */
    class ConstCurveIterator : 
      public std::iterator<std::forward_iterator_tag, Curve> {
    public:
      
      /** Create an invalid iterator. */
      ConstCurveIterator() { }
      
      /** Dereference the iterator to get a constant Controller reference. */
      const Curve& operator*() const { return **m_iterator; }
      /** Dereference the iterator to get a constant Controller pointer. */
      const Curve* operator->() const { return *m_iterator; }
      /** Returns @c true if the two iterators refer to the same Curve. */
      bool operator==(const ConstCurveIterator& iter) const {
        return (m_iterator == iter.m_iterator);
      }
      /** Returns @c false if the two iterators refer to the same Controller. */
      bool operator!=(const ConstCurveIterator& iter) const {
        return (m_iterator != iter.m_iterator);
      }
      /** Advances the iterator to the next controller. */
      ConstCurveIterator& operator++() { ++m_iterator; return *this; }
      
    private:
      
      friend class Pattern;

      ConstCurveIterator(const std::vector<Curve*>::iterator& iter) 
        : m_iterator(iter) { 
      }
      
      std::vector<Curve*>::iterator m_iterator;
    };


    /** Create a new pattern. */
    Pattern(int id, const std::string& name, 
	    const SongTime& length, int steps);
    /** Create a new pattern as a copy of @c pat. */
    Pattern(int id, const Pattern& pat);
    
    ~Pattern();
  
    /// @name Accessors
    //@{
    int get_id() const;
    /** Return the name of this pattern. */
    const std::string& get_name() const;
    const std::string& get_label() const;
    /** Return an iterator that refers to the first note in the pattern. */
    NoteIterator notes_begin() const;
    /** Return an invalid iterator that can be used to check when an iterator
        has passed the last note in this pattern. */
    NoteIterator notes_end() const;
    /** Return an iterator for the note with key @c value that is playing at
        step @c step, or an invalid iterator if there is no such note. */
    NoteIterator find_note(const SongTime& step, int value) const;
    /** Return an iterator that refers to the first controller in the pattern.*/
    ConstCurveIterator curves_begin() const;
    /** Return an invalid iterator that can be used to check when an iterator
        has passed the last controller in the pattern. */
    ConstCurveIterator curves_end() const;
    /** Return an iterator for the controller with parameter @c param, or an
        invalid iterator if no such controller exists. */
    ConstCurveIterator curves_find(long param) const;
    /** Return an iterator that refers to the first controller in the pattern.*/
    CurveIterator curves_begin();
    /** Return an invalid iterator that can be used to check when an iterator
        has passed the last controller in the pattern. */
    CurveIterator curves_end();
    /** Return an iterator for the controller with parameter @c param, or an
        invalid iterator if no such controller exists. */
    CurveIterator curves_find(long param);
    /** Return the number of steps per beat. */
    unsigned int get_steps() const;
    /** Return the length in beats. */
    const SongTime& get_length() const;
    /** Return the bounding rectangle in the (step, key) plane for all notes
        that have been changed since the last call to reset_dirty_rect(). */
    void get_dirty_rect(int* min_step, int* min_note, 
                        int* max_step, int* max_note) const;
    /** Check how long a note added at the given key and step could be. */
    const SongTime check_maximal_free_space(const SongTime& start, int key, 
					    const SongTime& limit) const;
    /** Check if it would be possible to add a note with the given start time,
	key and length. */
    bool check_free_space(const SongTime& step, int key, 
			  const SongTime& length) const;
    /** Check if it would be possible to add a note with the given start time,
	key and length, assuming that the notes in @c ignore were removed. */
    bool check_free_space(const SongTime& step, int key, 
			  const SongTime& length,
			  const NoteSelection& ignore) const;
    
    //@}
    
    /// @name Mutators
    //@{
    /** Set the name of this pattern. */
    void set_name(const std::string& name);
    /** Change the length in beats. */
    void set_length(const SongTime& length);
    /** Change the number of steps per beat. */
    void set_steps(unsigned int steps);
    /** Add a note at the given time with the given key, velocity, and 
	length. */
    NoteIterator add_note(const SongTime& start, int key, int velocity, 
			  const SongTime& length);
    /** Add a collection of notes with the given step and key offsets. 
        Can be used as a "paste" command. */
    bool add_notes(const NoteCollection& notes, const SongTime& start, 
		   int key, NoteSelection* selection = 0);
    /** Delete a note. */
    void delete_note(NoteIterator note);
    /** Change the length of a note. */
    int resize_note(NoteIterator note, int length);
    /** Set the velocity of a note. */
    void set_velocity(NoteIterator note, unsigned char velocity);
    /** Add a new parameter curve. */
    CurveIterator add_curve(const ControllerInfo& info);
    /** Add a parameter curve object. The length must match the number of
	steps in the pattern. */
    CurveIterator add_curve(Curve* curve);
    /** Remove a parameter curve. */
    bool remove_curve(CurveIterator iter);
    /** Remove a parameter curve from the pattern, but return a pointer to
	it instead of deleting it. The caller is responsible for deallocating
	the Curve object. */
    Curve* disown_curve(CurveIterator iter);
    /** Add a CC event to the given controller. */
    void add_curve_point(CurveIterator iter, const SongTime& step, int value);
    /** Remove a CC event. */
    void remove_curve_point(CurveIterator iter, const SongTime& step);
    /** Reset the "dirty rect".
        @see get_dirty_rect(). */
    void reset_dirty_rect() const; // XXX the "dirty rect" functions need to go
    //@}
    
    /// @name XML I/O
    //@{
    bool is_dirty() const;
    void make_clean() const;
    bool fill_xml_node(xmlpp::Element* elt) const;
    bool parse_xml_node(const xmlpp::Element* elt);
    //@}
    
    /// @name Sequencing
    //@{
    void sequence(MIDIBuffer& buffer, const SongTime& from, const SongTime& to,
                  const SongTime& pattern_length, int channel) const;
    //@}
    
  public:
    
    /// @name Signals
    //@{
    /** Emitted when the pattern name has changed. */
    sigc::signal<void, std::string>& signal_name_changed() const;
    /** Emitted when the length in beats has changed. */
    sigc::signal<void, SongTime const&>& signal_length_changed() const;
    /** Emitted when the number of steps per beat has changed. */
    sigc::signal<void, int>& signal_steps_changed() const;
    /** Emitted when a note has been added. */
    sigc::signal<void, Event const&>& signal_note_added() const;
    /** Emitted when an existing note has been changed. */
    sigc::signal<void, Event const&>& signal_note_changed() const;
    /** Emitted when a note has been removed. */
    sigc::signal<void, Event const&>& signal_note_removed() const;
    /** Emitted when a whole controller has been added. */
    sigc::signal<void, int>& signal_curve_added() const;
    /** Emitted when a whole controller has been removed. */
    sigc::signal<void, int>& signal_curve_removed() const;
    //@}
    
  private:
    
    typedef std::vector<NoteEvent*> NoteEventList;
    
    /** This struct is used internally so we can swap all data used by the
        sequencer with a single pointer assignment (for lock-free 
        thread safety). */
    /** We shouln't need this anymore, all sequencer structures will be 
	lock-free. */
    /*
    struct SeqData {
      SeqData(NoteEventList* note_ons, NoteEventList* note_offs, 
              std::vector<Curve*>* controllers,
              const SongTime& l, unsigned int s);
      ~SeqData();
      
      NoteEventList* ons;
      NoteEventList* offs;
      std::vector<Curve*>* curves;
      SongTime length;
      unsigned int steps;
    };
    */
    
    // no copying for now
    Pattern(const Pattern&) { assert(0); }
    Pattern& operator=(const Pattern&) { assert(0); return *this; }
    
    /** Find a note on event with the specified @c key between the steps
        @c start and @c end. */
    NoteIterator find_note_on(unsigned start, unsigned end, unsigned char key);
    /** Delete a note in a safe way. */
    void delete_note(EventList<4, 4>::Node* note);
    /** Resize a note. */
    int resize_note(Note* note, int length);
    
    /** The pattern id. */
    int m_id;
    /** The name of the pattern */
    std::string m_name;
    /** The data used by the sequencing functions need to be stored in a
        single structure so we can modify it in a lock-free way by swapping 
        a single pointer. */
    //SeqData* volatile m_sd;

    /** The actual events. This data structure is accessed by both threads,
	so be careful! */
    EventList<4, 4> m_events;
    
    mutable bool m_dirty;
  
    // dirty rect
    mutable int m_min_step, m_min_note, m_max_step, m_max_note;

    mutable sigc::signal<void, std::string> m_signal_name_changed;
    mutable sigc::signal<void, SongTime const&> m_signal_length_changed;
    mutable sigc::signal<void, int> m_signal_steps_changed;
    mutable sigc::signal<void, Event const&> m_signal_note_added;
    mutable sigc::signal<void, Event const&> m_signal_note_changed;
    mutable sigc::signal<void, Event const&> m_signal_note_removed;
    mutable sigc::signal<void, int> m_signal_curve_added;
    mutable sigc::signal<void, int> m_signal_curve_removed;
    
    
    EventList<4, 4> m_eventlist;
    
  };


}


#endif
