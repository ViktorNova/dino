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
#include <string>
#include <vector>

#include <sigc++/signal.h>
#include <libxml++/libxml++.h>

#include "controllerinfo.hpp"
#include "curve.hpp"
#include "xmlserialisable.hpp"
#include "sequencable.hpp"


namespace Dino {

  class InterpolatedEvent;
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
        access and modify data from curves in the pattern. */
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
    int get_id() const;
    const std::string& get_name() const;
    const std::string& get_label() const;
    int get_channel() const;
    unsigned int get_length() const;
    ConstPatternIterator pat_begin() const;
    ConstPatternIterator pat_end() const;
    ConstPatternIterator pat_find(int id) const;
    SequenceIterator seq_begin() const;
    SequenceIterator seq_end() const;
    SequenceIterator seq_find(unsigned int beat) const;
    SequenceIterator seq_find_by_id(int id) const;
    /** Return an iterator that refers to the first controller in the track.*/
    ConstCurveIterator curves_begin() const;
    /** Return an invalid iterator that can be used to check when an iterator
        has passed the last curve in the track. */
    ConstCurveIterator curves_end() const;
    /** Return an iterator for the controller with parameter @c param, or an
        invalid iterator if no such controller exists. */
    ConstCurveIterator curves_find(long param) const;
    const std::vector<ControllerInfo*>& get_controllers() const;
    Curve* get_curve(long number);
    
    // non-const accessors
    PatternIterator pat_begin();
    PatternIterator pat_end();
    PatternIterator pat_find(int id);
    /** Return an iterator that refers to the first controller in the track.*/
    CurveIterator curves_begin();
    /** Return an invalid iterator that can be used to check when an iterator
        has passed the last controller in the track. */
    CurveIterator curves_end();
    /** Return an iterator for the controller with parameter @c param, or an
        invalid iterator if no such controller exists. */
    CurveIterator curves_find(long param);
    std::vector<ControllerInfo*>& get_controllers();
    //@}
    
    /// @name Mutators
    //@{
    void set_name(const std::string& name);
    PatternIterator add_pattern(const std::string& name, int length, int steps);
    PatternIterator duplicate_pattern(ConstPatternIterator iterator);
    void remove_pattern(int id);
    SequenceIterator set_sequence_entry(int beat, int pattern, 
                                        unsigned int length = 0);
    void set_seq_entry_length(SequenceIterator iterator, unsigned int length);
    bool remove_sequence_entry(SequenceIterator iterator);
    void set_length(int length);
    void set_channel(int channel);
    bool add_controller(long number, const std::string& name, int default_v,
			int min, int max, bool global);
    bool remove_controller(long number);
    void set_controller_name(long number, const std::string& name);    
    void set_controller_min(long number, int min);    
    void set_controller_max(long number, int max);    
    void set_controller_default(long number, int _default);    
    void set_controller_number(long number, long new_number);    
    void set_controller_global(long number, bool global);    
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
    sigc::signal<void, const std::string&>& signal_name_changed();
    sigc::signal<void, int>& signal_pattern_added();
    sigc::signal<void, int>& signal_pattern_removed();
    sigc::signal<void, int, int, int>& signal_sequence_entry_added();
    sigc::signal<void, int, int, int>& signal_sequence_entry_changed();
    sigc::signal<void, int>& signal_sequence_entry_removed();
    sigc::signal<void, int>& signal_length_changed();
    sigc::signal<void, long>& signal_controller_added();
    sigc::signal<void, long>& signal_controller_removed();
    sigc::signal<void, long>& signal_controller_changed();
    sigc::signal<void, long>& signal_curve_added();
    sigc::signal<void, long>& signal_curve_removed();
    //@}
    
  private:
  
    int m_id;
    std::string m_name;
    std::map<int, Pattern*> m_patterns;
    std::vector<ControllerInfo*> m_controllers;
    int m_next_sid;
    volatile int m_channel;
    std::vector<SequenceEntry*>* volatile m_sequence;
    std::vector<Curve*>* volatile m_curves;
    
    mutable bool m_dirty;
  
    sigc::signal<void, const std::string&> m_signal_name_changed;
    sigc::signal<void, int> m_signal_pattern_added;
    sigc::signal<void, int> m_signal_pattern_removed;
    sigc::signal<void, int, int, int> m_signal_sequence_entry_added;
    sigc::signal<void, int, int, int> m_signal_sequence_entry_changed;
    sigc::signal<void, int> m_signal_sequence_entry_removed;
    sigc::signal<void, int> m_signal_length_changed;
    sigc::signal<void, long> m_signal_controller_added;
    sigc::signal<void, long> m_signal_controller_removed;
    sigc::signal<void, long> m_signal_controller_changed;
    sigc::signal<void, long> m_signal_curve_added;
    sigc::signal<void, long> m_signal_curve_removed;

  };


}


#endif
