#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include <string>

#include <asoundlib.h>
#include <glibmm/thread.h>
#include <jack/jack.h>

#include "song.hpp"


using namespace std;


class Sequencer {
public:
  
  /** This struct contains information about a writable MIDI port. */
  struct InstrumentInfo {
    string name;
    int client;
    int port;
  };
  
  
  Sequencer(const string& client_name, Song& song);
  ~Sequencer();
  
  // transport functions
  void play();
  void stop();
  void go_to_beat(double beat);
  
  bool is_valid() const;
  int get_alsa_id() const;
  InstrumentInfo get_first_instrument();
  InstrumentInfo get_next_instrument();
  void set_instrument(int track, int client, int port);
  
private:
  
  bool init_jack(const string& client_name);
  bool init_alsa(const string& client_name);
  
  void sequencing_loop();
  
  // JACK callbacks
  int jack_sync_callback(jack_transport_state_t state, jack_position_t* pos);
  void jack_timebase_callback(jack_transport_state_t state, 
			      jack_nframes_t nframes, jack_position_t* pos, 
			      int new_pos);
  
  // JACK callback wrappers
  static int jack_sync_callback_(jack_transport_state_t state, 
				 jack_position_t* pos, void* arg) {
    return static_cast<Sequencer*>(arg)->jack_sync_callback(state, pos);
  }
  static void jack_timebase_callback_(jack_transport_state_t state,
				      jack_nframes_t nframes,
				      jack_position_t* pos, int new_pos,
				      void* arg) {
    static_cast<Sequencer*>(arg)->jack_timebase_callback(state, nframes, pos,
							 new_pos);
  }

  
  string m_client_name;
  /** No one is allowed to read or write anything in this variable without
      locking m_song.get_big_mutex() - the exception is the list of tempo 
      changes, which always must be readable for the JACK timebase callback. */
  Song& m_song;
  /** This is @c true if JACK and ALSA has been initialised succesfully. */
  bool m_valid;
  /** This enum defines the different sync states the sequencer may be in.
      When the sequencer has been created and the JACK sync callback hasn't
      been called yet, it will be @c Waiting. When the sync callback is called
      it will change the value to @c Syncing, and next time the SEQ thread
      wakes up it will do the sync and change the value to @c SyncDone.
      When the JACK sync callback runs next time it will see this, change the
      value to @c InSync, and tell JACK that we're ready to go. */
  enum {
    /** The initial value - we haven't heard anything from JACK yet */
    Waiting,
    /** Set by the JACK thread when it's time to sync */
    Syncing,
    /** Set by the SEQ thread when the sync is done */
    SyncDone, 
    /** Set by the JACK thread to acknowledge that the sync is done */
    InSync    
  } m_sync_state;
  
  jack_client_t* m_jack_client;
  snd_seq_t* m_alsa_client;
  int m_alsa_queue;
  Thread* m_seq_thread;
};


#endif
