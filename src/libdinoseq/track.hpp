#ifndef TRACK_HPP
#define TRACK_HPP

#include <string>
#include <vector>

#include <sigc++/signal.h>
#include <libxml++/libxml++.h>

#include "xmlserialisable.hpp"


using namespace Glib;
using namespace sigc;
using namespace std;
using namespace xmlpp;


namespace Dino {

  class MIDIEvent;
  class Pattern;


  /** This class represents a track, which holds information about an instrument
      and how that instrument is played. It has a list of patterns, and a
      sequence that says when those patterns are played. 
  */
  class Track : public XMLSerialisable {
  public:
    
    /** This struct contains information about a sequence entry, i.e. a
	scheduled start and length for a pattern. */
    struct SequenceEntry {
      SequenceEntry(int patID, Pattern* patPtr, 
		    unsigned int st, unsigned int len) 
	: pattern_id(patID), pattern(patPtr), start(st), length(len) { }
      int pattern_id;
      Pattern* pattern;
      unsigned int start;
      unsigned int length;
    };
    
    
    /** This is an iterator class that is used to access the sequence entries
	in this track. It can not be used to modify the sequence entries,
	for that you should use the member functions in Track.
	@see Track::set_sequence_entry(), Track::set_seq_entry_length(),
	     Track::remove_sequence_entry() 
    */
    class SequenceIterator {
    public:
      SequenceIterator();
      const SequenceEntry* operator->() const;
      const SequenceEntry& operator*() const;
      bool operator==(const SequenceIterator& iter) const;
      bool operator!=(const SequenceIterator& iter) const;
      SequenceIterator& operator++();
      
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
    class PatternIterator {
    public:
      PatternIterator();
      const Pattern* operator->() const;
      const Pattern& operator*() const;
      Pattern* operator->();
      Pattern& operator*();
      bool operator==(const PatternIterator& iter) const;
      bool operator!=(const PatternIterator& iter) const;
      PatternIterator& operator++();
      
    private:
      
      friend class Track;
      friend class ConstPatternIterator;
      
      PatternIterator(const std::map<int, Pattern*>::iterator& iter);
      
      std::map<int, Pattern*>::iterator m_iter;
    };
    
    
    /** A read-only iterator class that can be used to access the patterns in
	this track. */
    class ConstPatternIterator {
    public:
      ConstPatternIterator();
      ConstPatternIterator(const PatternIterator& iter);
      const Pattern* operator->() const;
      const Pattern& operator*() const;
      bool operator==(const ConstPatternIterator& iter) const;
      bool operator!=(const ConstPatternIterator& iter) const;
      ConstPatternIterator& operator++();
      
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
    ConstPatternIterator pat_begin() const;
    ConstPatternIterator pat_end() const;
    ConstPatternIterator pat_find(int id) const;
    SequenceIterator seq_begin() const;
    SequenceIterator seq_end() const;
    SequenceIterator seq_find(unsigned int beat) const;
    int get_channel() const;
    unsigned int get_length() const;
    
    // non-const accessors
    PatternIterator pat_begin();
    PatternIterator pat_end();
    PatternIterator pat_find(int id);
    //@}
    
    /// @name Mutators
    //@{
    void set_name(const string& name);
    int add_pattern(const string& name, int length, int steps);
    void remove_pattern(int id);
    SequenceIterator set_sequence_entry(int beat, int pattern, int length = -1);
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
    int get_events(double beat, double before_beat,
		   const MIDIEvent** events, double* beats, int room) const;
    //@}
    
  public:
    
    /// @name Signals
    //@{
    signal<void, const string&> signal_name_changed;
    signal<void, int> signal_pattern_added; 
    signal<void, int> signal_pattern_removed;
    signal<void, int, int, int> signal_sequence_entry_added;
    signal<void, int, int, int> signal_sequence_entry_changed;
    signal<void, int> signal_sequence_entry_removed;
    signal<void, int> signal_length_changed;
    //@}
    
  private:
  
    int m_id;
    string m_name;
    map<int, Pattern*> m_patterns;
    volatile int m_channel;
    vector<SequenceEntry*>* volatile m_sequence;
  
    mutable bool m_dirty;
  
  };


}


#endif
