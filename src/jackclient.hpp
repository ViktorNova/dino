#ifndef JACKCLIENT_HPP
#define JACKCLIENT_HPP


#include <string>

#include <jack/jack.h>


using namespace std;


class JackClient {
public:
  
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
  
  
  JackClient(string client_name);
  
  void transport_start();
  void transport_stop();
  void transport_reposition(jack_position_t* pos);
  jack_transport_state_t transport_query(jack_position_t* pos);
  jack_nframes_t get_current_transport_frame();
  
  void set_bpm(double bpm);
  double get_bpm() const;
  void set_bpb(int bpb);
  int get_bpb() const;
  void set_tpb(int tpb);
  int get_tpb() const;
  jack_nframes_t get_sample_rate() const;
  void set_sync_state(SyncState state);
  SyncState get_sync_state() const;
  void set_last_timebase(int last_beat, int last_tick, 
			 jack_nframes_t last_frame);
  
private:
  
  // JACK callbacks
  int jack_sync_callback(jack_transport_state_t state, jack_position_t* pos);
  void jack_timebase_callback(jack_transport_state_t state, 
			      jack_nframes_t nframes, jack_position_t* pos, 
			      int new_pos);
  
  // JACK callback wrappers
  static int jack_sync_callback_(jack_transport_state_t state, 
				 jack_position_t* pos, void* arg) {
    return static_cast<JackClient*>(arg)->jack_sync_callback(state, pos);
  }
  static void jack_timebase_callback_(jack_transport_state_t state,
				      jack_nframes_t nframes,
				      jack_position_t* pos, int new_pos,
				      void* arg) {
    static_cast<JackClient*>(arg)->jack_timebase_callback(state, nframes, 
							  pos, new_pos);
  }

  

  jack_client_t* m_jack_client;
  double m_bpm;
  int m_bpb;
  int m_tpb;
  SyncState m_sync_state;

  /** This variable contains the current BPM value. It is set by the sequencer
      thread and read by the JACK thread. */
  int m_last_beat;
  int m_last_tick;
  jack_nframes_t m_last_frame;

};


#endif
