#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include <string>

#include <asoundlib.h>
#include <jack/jack.h>


using namespace std;

class Song;


/** This class is the actual sequencer. It will run in two separate threads - 
    the GUI thread, where the rest of the program can communicate with it 
    (for example by telling it to play() or stop()) and the realtime thread
    that is created for us by Jack. The sequencing is done in 
    jackProcessCallback(), which is called once for each Jack cycle. This
    function, and thus all functions that it calls, must be realtime safe -
    they can not do any blocking operations such as I/O or waiting for mutexes.
*/
class Sequencer {
public:
  /** Create a new Sequencer object with the ALSA and Jack client name 
      @c client_name. */
  Sequencer(const string& client_name);
  ~Sequencer();
  
  /** Set the song that the sequencer should play. */
  void set_song(const Song& song);
  
  /** Start playing from the current position. */
  void play();
  /** Stop playing, but stay at the current position (like "pause" on a 
      CD player). */
  void stop();
  /** Go to the given position. If we are playing, start playing again when 
      we get there. */
  void goto_beat(double beat);
  
private:
  
  /** Initialise the ALSA client with the given client name. */
  int init_alsa(const string& client_name);
  /** Initialise the Jack client with the given client name. */
  int init_jack(const string& client_name);
  
  /** This function checks if we are in sync with Jack Transport, if not it
      updates the sequencer to be in sync. */
  bool check_sync();
  
  // ========== Jack callbacks ==========
  /** This function is called by libjack when Jack Transport wants to start 
      playing or change position. If we're not ready yet we return 0. */
  int jack_sync_callback(jack_transport_state_t state, jack_position_t* pos);
  /** This function is called by libjack when the Jack graph starts working
      on a new buffer. We use this as a timer callback for the sequencer. */
  int jack_process_callback(jack_nframes_t nframes);
  /** This function is called by libjack when Jack wants us to fill in the
      bar, beat, tick, and bpm values. */
  void jack_timebase_callback(jack_transport_state_t state, 
			      jack_nframes_t nframes, jack_position_t* pos, 
			      int new_pos);
  
  /** Schedule a note at the given beat and tick. Send the current beat and
      tick so that we can use relative time scheduling. This function
      will schedule a note on and a note off. */
  void schedule_note(int beat, int tick, int value, int length, 
		    int currentBeat, int currentTick);
  
  // static wrappers for Jack callbacks - because we can't register member
  // functions directly
  static int jack_sync_callback_(jack_transport_state_t state, 
				 jack_position_t* pos, void* arg) {
    return static_cast<Sequencer*>(arg)->jack_sync_callback(state, pos);
  }
  static int jack_process_callback_(jack_nframes_t nframes, void* arg) {
    return static_cast<Sequencer*>(arg)->jack_process_callback(nframes);
  }
  static void jack_timebase_callback_(jack_transport_state_t state,
				      jack_nframes_t nframes,
				      jack_position_t* pos, int new_pos,
				      void* arg) {
    static_cast<Sequencer*>(arg)->jack_timebase_callback(state, nframes, pos,
							 new_pos);
  }

  
  const Song* m_song;
  
  jack_client_t* m_jack_client;
  snd_seq_t* m_alsa_client;
  int m_alsa_port;
  int m_alsa_queue;

  bool m_valid;

  volatile enum {
    InSync,
    Syncing,
    SyncDone
  } m_sync_state;
  int m_sync_to_beat;
  int m_sync_to_tick;
};


#endif
