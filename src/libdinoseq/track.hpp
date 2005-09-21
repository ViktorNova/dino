#ifndef TRACK_HPP
#define TRACK_HPP

#include <string>
#include <vector>

#include <sigc++/signal.h>
#include <libxml++/libxml++.h>

#include "deletable.hpp"


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
  class Track : public Deletable {
  public:
  
    /** This struct contains information about a sequence entry, i.e. a
	scheduled start and length for a pattern. */
    struct SequenceEntry : public Deletable {
      SequenceEntry(int patID, Pattern* patPtr, 
		    unsigned int st, unsigned int len) 
	: pattern_id(patID), pattern(patPtr), start(st), length(len) { }
      int pattern_id;
      Pattern* pattern;
      unsigned int start;
      unsigned int length;
    };
  
  
    Track(int length = 0, const string& name = "Untitled");
  
    ~Track();
  
    // accessors
    const string& get_name() const;
    map<int, Pattern*>& get_patterns();
    const map<int, Pattern*>& get_patterns() const;
    Pattern* get_pattern(int id);
    //const vector<SequenceEntry*>& get_sequence() const;
    const SequenceEntry* get_seq_entry(unsigned int beat);
    int get_channel() const;
    unsigned int get_length() const;
  
    // mutators
    void set_name(const string& name);
    int add_pattern(const string& name, int length, int steps, int ccSteps);
    void remove_pattern(int id);
    void set_sequence_entry(int beat, int pattern, int length = -1);
    void set_seq_entry_length(unsigned int beat, unsigned int length);
    bool remove_sequence_entry(int beat);
    void set_length(int length);
    void set_channel(int channel);
  
    // XML I/O
    bool is_dirty() const;
    void make_clean() const;
    bool fill_xml_node(Element* elt) const;
    bool parse_xml_node(const Element* elt);
  
    // sequencing
    MIDIEvent* get_events(unsigned int& beat, unsigned int& tick, 
			  unsigned int before_beat, unsigned int before_tick, 
			  unsigned int ticks_per_beat,
			  unsigned int& list) const;
  
  public:

    signal<void, const string&> signal_name_changed;
    signal<void, int> signal_pattern_added; 
    signal<void, int> signal_pattern_removed;
    signal<void, int, int, int> signal_sequence_entry_added;
    signal<void, int, int, int> signal_sequence_entry_changed;
    signal<void, int> signal_sequence_entry_removed;
    signal<void, int> signal_length_changed;
  
  private:
  
    string m_name;
    map<int, Pattern*> m_patterns;
    int m_length;
    volatile int m_channel;
    vector<SequenceEntry*> m_sequence;
  
    mutable bool m_dirty;
  
  };


}


#endif
