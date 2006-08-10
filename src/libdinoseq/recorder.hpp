#ifndef RECORDER_HPP
#define RECORDER_HPP

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
    
    int m_track;
  };

}


#endif
