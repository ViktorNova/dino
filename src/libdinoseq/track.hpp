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

#ifndef TRACK_HPP
#define TRACK_HPP

#include <iterator>
#include <string>
#include <vector>

#include <sigc++/signal.h>
#include <libxml++/libxml++.h>

#include "controllerinfo.hpp"
#include "xmlserialisable.hpp"


using namespace Glib;
using namespace sigc;
using namespace std;
using namespace xmlpp;


namespace Dino {

  class InterpolatedEvent;
  class MIDIBuffer;
  class Pattern;


  /** This class represents a track, which holds information about an instrument
      and how that instrument is played. It has a list of patterns, and a
      sequence that says when those patterns are played. 
  */
  class Track : public XMLSerialisable {
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
    
  
    Track(int id, int length = 0, const string& name = "Untitled");
  
    ~Track();
  
    /// @name Accessors
    //@{
    int get_id() const;
    const string& get_name() const;
    int get_channel() const;
    unsigned int get_length() const;
    ConstPatternIterator pat_begin() const;
    ConstPatternIterator pat_end() const;
    ConstPatternIterator pat_find(int id) const;
    SequenceIterator seq_begin() const;
    SequenceIterator seq_end() const;
    SequenceIterator seq_find(unsigned int beat) const;
    SequenceIterator seq_find_by_id(int id) const;
    
    // non-const accessors
    PatternIterator pat_begin();
    PatternIterator pat_end();
    PatternIterator pat_find(int id);
    //@}
    
    /// @name Mutators
    //@{
    void set_name(const string& name);
    PatternIterator add_pattern(const string& name, int length, int steps);
    PatternIterator duplicate_pattern(ConstPatternIterator iterator);
    void remove_pattern(int id);
    SequenceIterator set_sequence_entry(int beat, int pattern, 
                                        unsigned int length = 0);
    void set_seq_entry_length(SequenceIterator iterator, unsigned int length);
    bool remove_sequence_entry(SequenceIterator iterator);
    void set_length(int length);
    void set_channel(int channel);
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
    void sequence(MIDIBuffer& buffer, double from, double to) const;
    //@}
    
  public:
    
    /// @name Signals
    //@{
    signal<void, const string&>& signal_name_changed();
    signal<void, int>& signal_pattern_added();
    signal<void, int>& signal_pattern_removed();
    signal<void, int, int, int>& signal_sequence_entry_added();
    signal<void, int, int, int>& signal_sequence_entry_changed();
    signal<void, int>& signal_sequence_entry_removed();
    signal<void, int>& signal_length_changed();
    //@}
    
  private:
  
    int m_id;
    string m_name;
    map<int, Pattern*> m_patterns;
    vector<ControllerInfo> m_controllers;
    int m_next_sid;
    volatile int m_channel;
    vector<SequenceEntry*>* volatile m_sequence;
    
    mutable bool m_dirty;
  
    signal<void, const string&> m_signal_name_changed;
    signal<void, int> m_signal_pattern_added;
    signal<void, int> m_signal_pattern_removed;
    signal<void, int, int, int> m_signal_sequence_entry_added;
    signal<void, int, int, int> m_signal_sequence_entry_changed;
    signal<void, int> m_signal_sequence_entry_removed;
    signal<void, int> m_signal_length_changed;

  };


}


#endif
