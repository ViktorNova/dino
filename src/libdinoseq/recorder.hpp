/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006-2007  Lars Luthman <lars.luthman@gmail.com>
   
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

#ifndef RECORDER_HPP
#define RECORDER_HPP

#include <map>

#include <sigc++/signal.h>
#include <jack/jack.h>
#include <jack/transport.h>

#include "pattern.hpp"
#include "ringbuffer.hpp"


namespace Dino {
  

  class Song;  
  class Track;
  
  
  /** This class handles recording to a Track. It is used by the Sequencer
      class. */
  class Recorder {
  public:
    
    /** Create a new Recorder for the given Track. */
    Recorder(Song& song);
    
    /** Change the track to record to. May be 0, in which case nothing
        is recorded. */
    void set_track(int id);
    /** Get the track we are currently recording to. */
    int get_track();
    /** Run an iteration of the recorder in the editing thread. */
    void run_editing_thread();
    
    /** This is the only function in this class that may be called in the
        audio thread. */
    void run_audio_thread(jack_transport_state_t state, 
                          const jack_position_t& pos, 
                          jack_nframes_t nframes, void* input_buf,
                          jack_nframes_t rate);
    
    sigc::signal<void, int> signal_track_set;
    
  protected:
    
    enum CommandType {
      CommandTypeSetTrack
    };
    
    struct CommandSetTrack {
      int track;
    };
    
    struct Command {
      CommandType type;
      union {
        CommandSetTrack set_track;
      };
    };
    
    
    enum EventType {
      EventTypeTrackSet,
      EventTypeTick,
      EventTypeStart,
      EventTypeStop,
      EventTypeRelocation,
      EventTypeMIDI
    };
    
    struct EventTrackSet {
      int track;
    };
    
    struct EventTick {

    };
    
    struct EventStart {

    };
    
    struct EventStop {

    };
    
    struct EventRelocation {
      double to;
    };
    
    struct EventMIDI {
      unsigned char size;
      unsigned char data[3];
    };
    
    struct Event {
      EventType type;
      double beat;
      union {
        EventTrackSet track_set;
        EventTick tick_event;
        EventStart start;
        EventStop stop;
        EventRelocation relocation;
        EventMIDI midi;
      };
    };
    
    struct Key {
      Key() : held(false) { }
      bool held;
      int pattern;
      double start;
      unsigned char velocity;
      int seq_id;
    };
    
    void handle_note_on(double beat, unsigned char key, unsigned char velocity,
                        Track& track);
    void handle_note_off(double beat, unsigned char key, Track& track);
    void handle_controller(double beat, long param, int value, Track& track);
    void record_cc_point(Pattern& pat, 
                         Pattern::CurveIterator& citer, 
                         unsigned long step, int value);
    
    // variables that can only be accessed by the editing thread
    int m_track;
    Key m_keys[128];
    Song& m_song;
    int m_last_sid;
    std::map<long, int> m_ctrl_values;
    bool m_is_recording;
    double m_last_edit_beat;
    
    // variables that can only be accessed by the audio thread
    bool m_track_set;
    int m_last_tick;
    jack_transport_state_t m_last_state;
    double m_expected_beat;
    jack_nframes_t m_expected_frame;
    
    // commands from the editing thread to the audio thread
    Ringbuffer<Command> m_to_audio;
    
    // events from the audio thread to the editing thread
    Ringbuffer<Event> m_from_audio;
  };

}


#endif
