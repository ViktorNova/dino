#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include <map>
#include <string>
#include <vector>

#include <sigc++/signal.h>
#include <jack/jack.h>


using namespace std;


namespace Dino {


  class BaseMIDIEvent;
  class Song;


  /** This class sequences a Song to a string of MIDI events that are
      sent to JACK MIDI ports. It has functions for starting and stopping
      playback, changing the song position, and also for checking which
      instruments are available and for connecting a Track in the Song
      to one of those instruments. */
  class Sequencer {
  public:
  
    /** This struct contains information about a writable MIDI port. */
    struct InstrumentInfo {
      InstrumentInfo(const string& str) : name(str), connected(false) { }
      InstrumentInfo(const char* str) : name(str), connected(false) { }
      string name;
      bool connected;
    };
  
    /** This will create a new Sequencer object with the JACK client name 
	@c client_name and the Song object @c song. */
    Sequencer(const string& client_name, Song& song);
    ~Sequencer();
  
    // transport functions
    /** This starts the MIDI playback. */
    void play();
    /** This stops the MIDI playback. */
    void stop();
    /** This seeks to the given beat in the song. */
    void go_to_beat(double beat);
  
    /** Returns @c true if this Sequencer object is valid. */
    bool is_valid() const;
    /** This returns a vector of all instruments that are available for
	the sequencer to play. */
    vector<InstrumentInfo> get_instruments(int track = -1) const;
    /** This assigns the given instrument to the given track. */
    void set_instrument(int track, const string& instrument);
    /** This creates new MIDI output ports for all tracks. */
    void reset_ports();
  
    sigc::signal<void, int> signal_beat_changed;
  
  private:
  
    bool beat_checker();
  
    /** This initialises the internal JACK client object. */
    bool init_jack(const string& client_name);
  
    // JACK callbacks
    /** This function is called whenever the JACK daemon wants to know the
	beat and tick for a given frame position. */
    void jack_timebase_callback(jack_transport_state_t state, 
				jack_nframes_t nframes, jack_position_t* pos, 
				int new_pos);
    /** This is called once for each JACK cycle. MIDI is sequenced from here. */
    int jack_process_callback(jack_nframes_t nframes);
    /** This is called when the JACK daemon is being shut down. */
    void jack_shutdown_handler();
  
    // JACK callback wrappers
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
    static void jack_shutdown_handler_(void* arg) {
      static_cast<Sequencer*>(arg)->jack_shutdown_handler();
    }
  
    /** This function should be called when a new track has been added to the
	song. */
    void track_added(int track);
    /** This function should be called when a track has been removed from the
	song. */
    void track_removed(int track);
  
    /** This function does the actual MIDI playback (i.e. it puts the MIDI
	events on the JACK MIDI output buffers). */
    void sequence_midi(jack_transport_state_t state,
		       const jack_position_t& pos, jack_nframes_t nframes);
    /** This function adds a MIDI event to a JACK MIDI output buffer. It is
	used by sequence_midi(). */
    bool add_event_to_buffer(BaseMIDIEvent* event, void* port_buf,
			     unsigned int beat, unsigned int tick,
			     const jack_position_t& pos, jack_nframes_t nframes);
  
    string m_client_name;
    /* XXX The below is false, I think. Everything should be readable by
       the audio thread at all times without locking any mutii. */
    /** No one is allowed to read or write anything in this variable without
	locking m_song.get_big_mutex() - the exception is the list of tempo 
	changes, which always must be readable for the JACK timebase 
	callback. */
    Song& m_song;
    /** This is @c true if JACK and ALSA has been initialised succesfully. */
    bool m_valid;
  
    jack_client_t* m_jack_client;
    map<int, jack_port_t*> m_output_ports;
    jack_port_t* m_input_port;
    int m_last_beat;
    int m_last_tick;
    bool m_sent_all_off;
    static const int m_event_buffer_size = 1024;
    BaseMIDIEvent* m_event_buffer[m_event_buffer_size];
    
    volatile int m_current_beat;
    volatile int m_old_current_beat;
  };


}
#endif
