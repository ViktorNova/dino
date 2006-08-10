#ifndef RECORDER_HPP
#define RECORDER_HPP

#include <sigc++/signal.h>

#include "ringbuffer.hpp"


namespace Dino {
  
  
  class Track;
  
  
  /** This class handles recording to a Track. It is used by the Sequencer
      class. */
  class Recorder {
  public:
    
    /** Create a new Recorder for the given Track. */
    Recorder();
    
    /** Change the track to record to. May be 0, in which case nothing
        is recorded. */
    void set_track(int id);
    /** Get the track we are currently recording to. */
    int get_track();
    /** Run an iteration of the recorder in the editing thread. */
    void run_editing_thread();
    
    /** This is the only function in this class that may be called in the
        audio thread. */
    void run_audio_thread();
    
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
      EventTypeTrackSet
    };
    
    struct EventTrackSet {
      int track;
    };
    
    struct Event {
      EventType type;
      union {
        EventTrackSet track_set;
      };
    };
    
    // variables that can only be accessed by the editing thread
    int m_track;
    
    // variables that can only be accessed by the audio thread
    bool m_track_set;
    
    // commands from the editing thread to the audio thread
    Ringbuffer<Command> m_to_audio;
    
    // events from the audio thread to the editing thread
    Ringbuffer<Event> m_from_audio;
  };

}


#endif
