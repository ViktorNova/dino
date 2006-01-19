#ifndef PATTERN_H
#define PATTERN_H

#include <list>
#include <map>
#include <vector>


#include <sigc++/signal.h>
#include <libxml++/libxml++.h>

#include "controller.hpp"
#include "deletable.hpp"
#include "xmlserialisable.hpp"


using namespace Glib;
using namespace sigc;
using namespace std;
using namespace xmlpp;


namespace Dino {

  class NoteEvent;
  class MIDIEvent;
  

  /** This class stores information about a pattern. A pattern is a sequence of
      notes and MIDI control changes which can be played at a certain time
      by the sequencer. */
  class Pattern : public Deletable, public XMLSerialisable {
  public:
  
    Pattern(const string& name, int length, int steps);
  
    ~Pattern();
  
    typedef vector<NoteEvent*> NoteEventList;
  
    // accessors
    const string& get_name() const;
    const NoteEventList& get_notes() const;
    int get_steps() const;
    int get_length() const;
    void get_dirty_rect(int* min_step, int* min_note, 
			int* max_step, int* max_note);
    const std::vector<Controller>& get_controllers() const;
    NoteEvent* find_note(int step, int value);
    
    // mutators
    void set_name(const string& name);
    void add_note(int step, int value, int velocity, int length);
    int delete_note(NoteEvent* note_on);
    int resize_note(NoteEvent* note_on, int length);
  
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
    sigc::signal<void, int> signal_cc_steps_changed;
    sigc::signal<void, int, int, int> signal_note_added;
    sigc::signal<void, int, int, int> signal_note_changed;
    sigc::signal<void, int, int> signal_note_removed;
    sigc::signal<void, int, int, int> signal_cc_added;
    sigc::signal<void, int, int, int> signal_cc_changed;
    sigc::signal<void, int> signal_controller_added;
    sigc::signal<void, int> signal_controller_removed;
    sigc::signal<void, int, int> signal_cc_removed;

  private:
    
    // no copying for now
    Pattern(const Pattern&) { }
    Pattern& operator=(const Pattern&) { return *this; }
    
    bool find_note_event(int step, int value, bool note_on, NoteEvent*& event);
    void delete_note_event(NoteEvent* event);
    void add_note_event(NoteEvent* event);
  
    string m_name;
    /** Pattern length in beats */
    int m_length;
    /** Number of steps per beat */
    int m_steps;
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
