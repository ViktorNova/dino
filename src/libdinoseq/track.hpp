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

#ifndef TRACK_HPP
#define TRACK_HPP

#include <iterator>
#include <map>
#include <string>
#include <vector>

#include <sigc++/signal.h>
#include <libxml++/libxml++.h>

#include "curve.hpp"
#include "xmlserialisable.hpp"
#include "sequencable.hpp"


namespace Dino {

  class ControllerInfo;
  class InterpolatedEvent;
  class KeyInfo;
  class MIDIBuffer;
  class Pattern;
  

  /** This class represents a track, which holds information about an instrument
      and how that instrument is played. It has a list of patterns, and a
      sequence that says when those patterns are played. 
  */
  class Track : public XMLSerialisable, public Sequencable {
  public:
    
    /** This struct contains information about a sequence entry, i.e. a
        scheduled start and length for a pattern. */
    class SequenceEntry {
    public:

      SequenceEntry(int patID, Pattern* patPtr, 
                    unsigned int st, unsigned int len) 
        : pattern_id(patID), pattern(patPtr), start(st), length(len), id(-1) { }
      
      int get_pattern_id() const { return pattern_id; }
      Pattern& get_pattern() { return *pattern; }
      unsigned int get_start() const { return start; }
      unsigned int get_length() const { return length; }
      int get_id() const { return id; }
      
    private:
      
      friend class Track;
      
      int pattern_id;
      Pattern* pattern;
      unsigned int start;
      unsigned int length;
      int id;
    };
    
    
    /** This is an iterator class that is used to access the sequence entries
        in this track. It can not be used to modify the sequence entries,
        for that you should use the member functions in Track.
        @see Track::set_sequence_entry(), Track::set_seq_entry_length(),
        Track::remove_sequence_entry() 
    */
    class SequenceIterator : 
      public std::iterator<std::forward_iterator_tag, SequenceEntry> {
    public:
      SequenceIterator();
      SequenceEntry* operator->();
      SequenceEntry& operator*();
      bool operator==(const SequenceIterator& iter) const;
      bool operator!=(const SequenceIterator& iter) const;
      SequenceIterator& operator++();
      SequenceIterator operator++(int);
      
    private:
      
      friend class Track;
      
      SequenceIterator(const std::vector<SequenceEntry*>::const_iterator& iter,
                       const std::vector<SequenceEntry*>& vec);
      
      std::vector<SequenceEntry*>::const_iterator m_iter;
      const std::vector<SequenceEntry*>* m_vector;
    };
    
    
    class ConstPatternIterator;
    
    
    /** An iterator class that can be used to access and modify the patterns
        in this track. */
    class PatternIterator : 
      public std::iterator<std::forward_iterator_tag, Pattern> {
    public:
      PatternIterator();
      const Pattern* operator->() const;
      const Pattern& operator*() const;
      Pattern* operator->();
      Pattern& operator*();
      bool operator==(const PatternIterator& iter) const;
      bool operator!=(const PatternIterator& iter) const;
      PatternIterator& operator++();
      PatternIterator operator++(int);
      
    private:
      
      friend class Track;
      friend class ConstPatternIterator;
      
      PatternIterator(const std::map<int, Pattern*>::iterator& iter);
      
      std::map<int, Pattern*>::iterator m_iter;
    };
    
    
    /** A read-only iterator class that can be used to access the patterns in
        this track. */
    class ConstPatternIterator 
      : public std::iterator<std::forward_iterator_tag, Pattern> {
    public:
      ConstPatternIterator();
      ConstPatternIterator(const PatternIterator& iter);
      const Pattern* operator->() const;
      const Pattern& operator*() const;
      bool operator==(const ConstPatternIterator& iter) const;
      bool operator!=(const ConstPatternIterator& iter) const;
      ConstPatternIterator& operator++();
      ConstPatternIterator operator++(int);
      
    private:
      
      friend class Track;
      
      ConstPatternIterator(const std::map<int, Pattern*>::const_iterator& iter);

      std::map<int, Pattern*>::const_iterator m_iter;
    };


    /** A CurveIterator is a iterator type that can be used to
        access and modify data from curves in the track. */
    class CurveIterator : 
      public std::iterator<std::forward_iterator_tag, Curve> {
    public:
      
      /** Create an invalid iterator. */
      CurveIterator() { }
      
      /** Dereference the iterator to get a Curve reference. */
      Curve& operator*() { return **m_iterator; }
      /** Dereference the iterator to get a Curve pointer. */
      Curve* operator->() { return *m_iterator; }
      /** Dereference the iterator to get a constant Curve reference. */
      const Curve& operator*() const { return **m_iterator; }
      /** Dereference the iterator to get a constant Curve pointer. */
      const Curve* operator->() const { return *m_iterator; }
      /** Returns @c true if the two iterators refer to the same Curve. */
      bool operator==(const CurveIterator& iter) const {
        return (m_iterator == iter.m_iterator);
      }
      /** Returns @c false if the two iterators refer to the same Curve. */
      bool operator!=(const CurveIterator& iter) const {
        return (m_iterator != iter.m_iterator);
      }
      /** Advances the iterator to the next curve. */
      CurveIterator& operator++() { ++m_iterator; return *this; }
      
    private:
      
      friend class Track;

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
      
      /** Dereference the iterator to get a constant Curve reference. */
      const Curve& operator*() const { return **m_iterator; }
      /** Dereference the iterator to get a constant Curve pointer. */
      const Curve* operator->() const { return *m_iterator; }
      /** Returns @c true if the two iterators refer to the same Curve. */
      bool operator==(const ConstCurveIterator& iter) const {
        return (m_iterator == iter.m_iterator);
      }
      /** Returns @c false if the two iterators refer to the same Curve. */
      bool operator!=(const ConstCurveIterator& iter) const {
        return (m_iterator != iter.m_iterator);
      }
      /** Advances the iterator to the next curve. */
      ConstCurveIterator& operator++() { ++m_iterator; return *this; }
      
    private:
      
      friend class Track;

      ConstCurveIterator(const std::vector<Curve*>::iterator& iter) 
        : m_iterator(iter) { 
      }
      
      std::vector<Curve*>::iterator m_iterator;
    };

    
    Track(int id, int length = 0, const std::string& name = "Untitled");
  
    ~Track();
  
    /// @name Accessors
    //@{
    /** Return the ID number of this track. This is not valid if the track does
	not belong to a Song. */
    int get_id() const;
    /** Return the name of this track. */
    const std::string& get_name() const;
    /** Alias for get_name(). */
    const std::string& get_label() const;
    /** Return the MIDI channel this track will write to. */
    int get_channel() const;
    /** Return the length of this track in beats. When it belongs to a Song 
	it should always be the same as the length of the Song. */
    unsigned int get_length() const;
    /** Return an iterator pointing to the first pattern. */
    ConstPatternIterator pat_begin() const;
    /** Return an (invalid) iterator pointing to the end of the pattern list. */
    ConstPatternIterator pat_end() const;
    /** Return an iterator for the pattern with the given ID, or pat_end() if
	there is no such pattern. */
    ConstPatternIterator pat_find(int id) const;
    /** Return an iterator pointing to the first sequence entry. */
    SequenceIterator seq_begin() const;
    /** Return an (invalid) iterator pointing to the end of the sequence. */
    SequenceIterator seq_end() const;
    /** Return an iterator pointing to the SequenceEntry at the given beat, 
	or seq_end() if there is no such SequenceEntry. */
    SequenceIterator seq_find(unsigned int beat) const;
    /** Return an iterator pointing to the SequenceEntry with the given ID,
	or seq_end() if there is no such SequenceEntry. */
    SequenceIterator seq_find_by_id(int id) const;
    /** Return an iterator that refers to the first controller in the track.*/
    ConstCurveIterator curves_begin() const;
    /** Return an invalid iterator that can be used to check when an iterator
        has passed the last curve in the track. */
    ConstCurveIterator curves_end() const;
    /** Return an iterator for the controller with parameter @c param, or an
        invalid iterator if no such controller exists. */
    ConstCurveIterator curves_find(long param) const;
    /** Return a vector of all controllers. */
    const std::vector<ControllerInfo*>& get_controllers() const;
    /** Return a vector of all named keys. */
    const std::vector<KeyInfo*>& get_keys() const;
    /** Return the curve with the given controller number. */
    Curve* get_curve(long number);
    
    // non-const accessors
    PatternIterator pat_begin();
    PatternIterator pat_end();
    PatternIterator pat_find(int id);
    /** Return an iterator that refers to the first global parameter curve in 
	the track.*/
    CurveIterator curves_begin();
    /** Return an invalid iterator that can be used to check when an iterator
        has passed the last parameter curve in the track. */
    CurveIterator curves_end();
    /** Return an iterator for the parameter curve with parameter @c param, or
	an invalid iterator if no such controller exists. */
    CurveIterator curves_find(long param);
    std::vector<ControllerInfo*>& get_controllers();
    //@}
    
    /// @name Mutators
    //@{
    /** Change the name of the track. */
    void set_name(const std::string& name);
    /** Add a new pattern to the track with the given name, length in beats
	and number of steps per beat. */
    PatternIterator add_pattern(const std::string& name, int length, int steps);
    /** Add an existing Pattern object to the track. The object must be 
	allocated using @c new, and the track will assume ownership of the 
	object. */
    PatternIterator add_pattern(Pattern* pattern);
    /** Duplicate the given pattern. */
    PatternIterator duplicate_pattern(ConstPatternIterator iterator);
    /** Remove the pattern with the given ID from the track and delete it. */
    void remove_pattern(int id);
    /** Remove the pattern with the given ID from the track and return a pointer
	to it. The caller is responsible for deallocating the Pattern object
	using @c delete (or Delete::queue() if the sequencer is running). */
    Pattern* disown_pattern(int id);
    /** Add a sequence entry. */
    SequenceIterator set_sequence_entry(int beat, int pattern, 
                                        unsigned int length = 0);
    /** Change the length of a sequence entry. */
    void set_seq_entry_length(SequenceIterator iterator, unsigned int length);
    /** Remove a sequence entry. */
    bool remove_sequence_entry(SequenceIterator iterator);
    /** Change the length of a track. This should not be used when the track
	belongs to a Song. */
    void set_length(int length);
    /** Set the MIDI channel that the track writes its events to. */
    void set_channel(int channel);
    /** Add a new controller to the track with the given parameters. */
    bool add_controller(long number, const std::string& name, int default_v,
			int min, int max, bool global);
    /** Add a new controller to the track using the given ControllerInfo object
	and Curve map. The lengths of the Curve objects must be equal to the
	number of steps in the patterns they are associated with. */
    bool add_controller(ControllerInfo* info, std::map<int, Curve*>& curves);
    /** Remove the controller with the given number and delete its 
	ControllerInfo and Curve objects. */
    bool remove_controller(long number);
    /** Remove the controller with the given number, return its ControllerInfo
	object, and add pointers to its Curve objects to @c curves. The caller
	is responsible for deallocating these objects using @c delete (or
	Delete::queue() if the sequencer thread is running). */
    ControllerInfo* disown_controller(long number, 
				      std::map<int, Curve*>& curves);
    /** Change the name of a controller. */
    void set_controller_name(long number, const std::string& name);    
    /** Change the minimum value of a controller. */
    void set_controller_min(long number, int min);
    /** Change the maximum value of a controller. */
    void set_controller_max(long number, int max);  
    /** Change the default value of a controller. */
    void set_controller_default(long number, int _default);    
    /** Change the parameter number of a controller. */
    void set_controller_number(long number, long new_number);    
    /** Change the 'global' toggle of a controller. */
    void set_controller_global(long number, bool global);    
    /** Add a named key to the track. */
    bool add_key(unsigned char number, const std::string& name);
    /** Remove a named key. */
    bool remove_key(unsigned char number);
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
    void sequence(MIDIBuffer& buffer, double from, double to,
		  unsigned int length, int channel) const;
    //@}
    
  public:
    
    /// @name Signals
    //@{
    sigc::signal<void, const std::string&>& signal_name_changed() const;
    sigc::signal<void, int>& signal_pattern_added() const;
    sigc::signal<void, int>& signal_pattern_removed() const;
    sigc::signal<void, int, int, int>& signal_sequence_entry_added() const;
    sigc::signal<void, int, int, int>& signal_sequence_entry_changed() const;
    sigc::signal<void, int>& signal_sequence_entry_removed() const;
    sigc::signal<void, int>& signal_length_changed() const;
    sigc::signal<void, long>& signal_controller_added() const;
    sigc::signal<void, long>& signal_controller_removed() const;
    sigc::signal<void, long>& signal_controller_changed() const;
    sigc::signal<void, long>& signal_curve_added() const;
    sigc::signal<void, long>& signal_curve_removed() const;
    //@}
    
  private:
  
    int m_id;
    std::string m_name;
    std::map<int, Pattern*> m_patterns;
    std::vector<ControllerInfo*> m_controllers;
    std::vector<KeyInfo*> m_keys;
    int m_next_sid;
    volatile int m_channel;
    std::vector<SequenceEntry*>* volatile m_sequence;
    std::vector<Curve*>* volatile m_curves;
    
    mutable bool m_dirty;
  
    mutable sigc::signal<void, const std::string&> m_signal_name_changed;
    mutable sigc::signal<void, int> m_signal_pattern_added;
    mutable sigc::signal<void, int> m_signal_pattern_removed;
    mutable sigc::signal<void, int, int, int> m_signal_sequence_entry_added;
    mutable sigc::signal<void, int, int, int> m_signal_sequence_entry_changed;
    mutable sigc::signal<void, int> m_signal_sequence_entry_removed;
    mutable sigc::signal<void, int> m_signal_length_changed;
    mutable sigc::signal<void, long> m_signal_controller_added;
    mutable sigc::signal<void, long> m_signal_controller_removed;
    mutable sigc::signal<void, long> m_signal_controller_changed;
    mutable sigc::signal<void, long> m_signal_curve_added;
    mutable sigc::signal<void, long> m_signal_curve_removed;
    
  };


}


#endif
