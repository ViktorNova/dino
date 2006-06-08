#ifndef RECORDER_HPP
#define RECORDER_HPP

#include "ringbuffer.hpp"


namespace Dino {
  
  
  class Track;
  
  
  /** This class handles recording to a Track. It is used bu the Sequencer
      class. */
  class Recorder {
  public:
    
    /** Create a new Recorder for the given Track. */
    Recorder();
    
    /** Change the track to record to. May be 0, in which case nothing
        is recorded. */
    void set_track(Track* track);
    /** Get the track we are currently recording to. */
    Track* get_track();
    /** Tell the recorder that we started recording at the given beat. */
    void start(double beat);
    /** Record a MIDI event. */
    void record_event(double beat, unsigned int size, unsigned char* data);
    /** Tell the recorder that we relocated. */
    void relocate(double from, double to);
    /** Tell the recorder that we stopped recording at the given beat. */
    void stop(double beat);
    
  protected:
    
    /** Do the actual recording (called in the editing thread). */
    void do_record();
    
    /** Record one note. */
    void record_note(unsigned char key, double end);
    
    /** The different types of recordable events */
    enum EventType {
      EVENT_MIDI,
      EVENT_START,
      EVENT_STOP,
      EVENT_RELOCATE
    };
    
    /** A recordable event. */
    struct Event {
      /** The type of recordable event. */
      EventType type;
      /** For MIDI, start, and stop events, this is the time stamp, for 
          relocation events this is the beat that we relocated from */
      double beat1;
      /** For relocation events, this is the beat that we relocated to. 
          Not used for other events. */
      double beat2;
      /** The used size of the MIDI data array for MIDI events. Not used for
          other events. */
      unsigned int size;
      /** The MIDI data array for MIDI events. Not used for other events. */
      unsigned char data[4];
    };
    
    /** A note waiting to be added. */
    struct WaitingNote {
      WaitingNote() : valid(false) { }
      /** Is this note actually waiting to be added? */
      bool valid;
      /** The beat the note will start on. */
      double start;
      /** The velocity of the note. */
      unsigned char velocity;
      /** The pattern ID that the note should be added to. */
      int pattern_id;
      /** The beat where the note should stop if a note off isn't received 
          earlier. */
      double end;
    };
    
    Ringbuffer<Event> m_events;
    WaitingNote m_notes[128];
    Track* m_track;
    
  };

}


#endif
