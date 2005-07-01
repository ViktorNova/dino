#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include <map>
#include <string>

#include <jack/jack.h>

#include <glibmm/thread.h>


using namespace std;
using namespace Glib;


class Song;


class Sequencer : public GObject {
public:
  
  /** This struct contains information about a writable MIDI port. */
  struct InstrumentInfo {
    string name;
    int client;
    int port;
  };
  
  /** This enum defines the different sync states the sequencer may be in.
      When the sequencer has been created and the JACK sync callback hasn't
      been called yet, it will be @c Waiting. When the sync callback is called
      it will change the value to @c Syncing, and next time the SEQ thread
      wakes up it will do the sync and change the value to @c SyncDone.
      When the JACK sync callback runs next time it will see this, change the
      value to @c InSync, and tell JACK that we're ready to go. */
  enum SyncState {
    /** The initial value - we haven't heard anything from JACK yet */
    Waiting,
    /** Set by the JACK thread when it's time to sync */
    Syncing,
    /** Set by the SEQ thread when the sync is done */
    SyncDone,
    /** Set by the JACK thread to acknowledge that the sync is done */
    InSync
  };

  
  Sequencer(const string& client_name, Song& song);
  ~Sequencer();
  
  // transport functions
  void play();
  void stop();
  void go_to_beat(double beat);
  
  bool is_valid() const;
  InstrumentInfo get_first_instrument();
  InstrumentInfo get_next_instrument();
  void set_instrument(int track, int client, int port);
  void reset_ports();
  
private:
  
  bool init_jack(const string& client_name);
  
  // JACK callbacks
  int jack_sync_callback(jack_transport_state_t state, jack_position_t* pos);
  void jack_timebase_callback(jack_transport_state_t state, 
			      jack_nframes_t nframes, jack_position_t* pos, 
			      int new_pos);
  int jack_process_callback(jack_nframes_t nframes);
  
  // JACK callback wrappers
  static int jack_sync_callback_(jack_transport_state_t state, 
				 jack_position_t* pos, void* arg) {
    return static_cast<Sequencer*>(arg)->jack_sync_callback(state, pos);
  }
  static void jack_timebase_callback_(jack_transport_state_t state,
				      jack_nframes_t nframes,
				      jack_position_t* pos, int new_pos,
				      void* arg) {
    static_cast<Sequencer*>(arg)->jack_timebase_callback(state, nframes, 
							 pos, new_pos);
  }
  static int jack_process_callback_(jack_nframes_t nframes, void* arg) {
    return static_cast<Sequencer*>(arg)->jack_process_callback(nframes);
  }
  
  void track_added(int track);
  void track_removed(int track);
  
  /** @xmlonly _realtime_safe */
  void sequencing_loop();
  void sequence_midi(jack_transport_state_t state,
		     const jack_position_t& pos, jack_nframes_t nframes);
  void add_event_to_buffer(Pattern::NoteEvent* event);
  //void play_midi();
  //void schedule_note(int beat, int tick, int port, int channel, 
  //		     int value, int velocity, int length);
  
  string m_client_name;
  /** No one is allowed to read or write anything in this variable without
      locking m_song.get_big_mutex() - the exception is the list of tempo 
      changes, which always must be readable for the JACK timebase callback. */
  Song& m_song;
  /** This is @c true if JACK and ALSA has been initialised succesfully. */
  bool m_valid;
  
  //Thread* m_seq_thread;
  jack_client_t* m_jack_client;
  SyncState m_sync_state;
  map<int, jack_port_t*> m_output_ports;
  jack_port_t* m_input_port;
};


#endif
