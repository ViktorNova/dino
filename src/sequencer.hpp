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
      @c clientName. */
  Sequencer(const string& clientName);
  ~Sequencer();
  
  /** Set the song that the sequencer should play. */
  void setSong(const Song& song);
  
  /** Start playing from the current position. */
  void play();
  /** Stop playing, but stay at the current position (like "pause" on a 
      CD player). */
  void stop();
  /** Go to the given position. If we are playing, start playing again when 
      we get there. */
  void gotoBeat(double beat);
  
private:
  
  /** Initialise the ALSA client with the given client name. */
  int initALSA(const string& clientName);
  /** Initialise the Jack client with the given client name. */
  int initJack(const string& clientName);
  
  /** This function checks if we are in sync with Jack Transport, if not it
      updates the sequencer to be in sync. */
  bool checkSync();
  
  // ========== Jack callbacks ==========
  /** This function is called by libjack when Jack Transport wants to start 
      playing or change position. If we're not ready yet we return 0. */
  int jackSyncCallback(jack_transport_state_t state, jack_position_t* pos);
  /** This function is called by libjack when the Jack graph starts working
      on a new buffer. We use this as a timer callback for the sequencer. */
  int jackProcessCallback(jack_nframes_t nframes);
  /** This function is called by libjack when Jack wants us to fill in the
      bar, beat, tick, and bpm values. */
  void jackTimebaseCallback(jack_transport_state_t state, 
			    jack_nframes_t nframes, jack_position_t* pos, 
			    int new_pos);
  
  /** Schedule a note at the given beat and tick. Send the current beat and
      tick so that we can use relative time scheduling. This function
      will schedule a note on and a note off. */
  void scheduleNote(int beat, int tick, int value, int length, 
		    int currentBeat, int currentTick);
  
  // static wrappers for Jack callbacks - because we can't register member
  // functions directly
  static int jackSyncCallback_(jack_transport_state_t state, 
			       jack_position_t* pos, void* arg) {
    return static_cast<Sequencer*>(arg)->jackSyncCallback(state, pos);
  }
  static int jackProcessCallback_(jack_nframes_t nframes, void* arg) {
    return static_cast<Sequencer*>(arg)->jackProcessCallback(nframes);
  }
  static void jackTimebaseCallback_(jack_transport_state_t state,
				    jack_nframes_t nframes,
				    jack_position_t* pos, int new_pos,
				    void* arg) {
    static_cast<Sequencer*>(arg)->jackTimebaseCallback(state, nframes, pos,
						       new_pos);
  }

  
  const Song* mSong;
  
  jack_client_t* jackClient;
  snd_seq_t* alsaClient;
  int alsaPort;
  int alsaQueue;

  bool mValid;

  volatile enum {
    InSync,
    Syncing,
    SyncDone
  } mSyncState;
  int mSyncToBeat;
  int mSyncToTick;
};


#endif
