#include <iostream>

#include "sequencer.hpp"



Sequencer::Sequencer(const string& client_name, Song& song) 
  : m_client_name(client_name), m_song(song), m_valid(false), 
    m_sync_state(Waiting) {

  if (!init_jack(m_client_name)) {
    cerr<<"Could not initialise JACK!"<<endl;
    return;
  }
  if (!init_alsa(m_client_name)) {
    cerr<<"Could not initialise ALSA!"<<endl;
    return;
  }
  m_valid = true;

  m_song.signal_track_added.connect(mem_fun(*this, &Sequencer::track_added));
  m_song.signal_track_removed.
    connect(mem_fun(*this, &Sequencer::track_removed));
  
  m_seq_thread = 
    Thread::create(mem_fun(*this, &Sequencer::sequencing_loop), true);
}


Sequencer::~Sequencer() {
  if (m_valid) {
    stop();
    go_to_beat(0);
    m_valid = false;
    m_seq_thread->join();
  }
}
  

void Sequencer::play() {
  if (m_valid)
    jack_transport_start(m_jack_client);
}


void Sequencer::stop() {
  if (m_valid)
    jack_transport_stop(m_jack_client);
}
 

void Sequencer::go_to_beat(double beat) {
  if (m_valid) {
    jack_position_t pos;
    memset(&pos, 0, sizeof(jack_position_t));
    jack_transport_reposition(m_jack_client, &pos);
  }
}
  
 
bool Sequencer::is_valid() const {
  return m_valid;
}


int Sequencer::get_alsa_id() const {
  if (m_valid) {
    snd_seq_client_info_t* seq_info;
    snd_seq_client_info_malloc(&seq_info);
    snd_seq_get_client_info(m_alsa_client, seq_info);
    int id = snd_seq_client_info_get_client(seq_info);
    snd_seq_client_info_free(seq_info);
    return id;
  }
  return -1;
}


Sequencer::InstrumentInfo Sequencer::get_first_instrument() {
  InstrumentInfo info = { "", -1, -1 };
  return info;
}
 

Sequencer::InstrumentInfo Sequencer::get_next_instrument() {
  InstrumentInfo info = { "", -1, -1 };
  return info;
}


void Sequencer::set_instrument(int track, int client, int port) {

}


void Sequencer::reset_ports() {
  map<int, Track*>::const_iterator iter = m_song.get_tracks().begin();
  for ( ; iter != m_song.get_tracks().end(); ++iter)
    track_added(iter->first);
}

  
bool Sequencer::init_jack(const string& client_name) {
  m_jack_client = jack_client_new(client_name.c_str());
  if (!m_jack_client)
    return false;
  int err;
  if ((err = jack_set_sync_callback(m_jack_client, 
				    &Sequencer::jack_sync_callback_,
				    this)) != 0)
    return false;
  if ((err = jack_set_timebase_callback(m_jack_client, 1, 
					&Sequencer::jack_timebase_callback_, 
					this)) != 0)
    return false;
  if ((err = jack_activate(m_jack_client)) != 0)
    return false;
  
  jack_position_t pos;
  memset(&pos, 0, sizeof(pos));
  jack_transport_stop(m_jack_client);
  jack_transport_reposition(m_jack_client, &pos);
  
  return true;
}


bool Sequencer::init_alsa(const string& client_name) {
  if (snd_seq_open(&m_alsa_client, "default", SND_SEQ_OPEN_DUPLEX, 0))
    return false;
  snd_seq_set_client_name(m_alsa_client, client_name.c_str());
  m_alsa_queue = snd_seq_alloc_queue(m_alsa_client);
  snd_seq_start_queue(m_alsa_client, m_alsa_queue, NULL);
  snd_seq_create_simple_port(m_alsa_client, "Input",
			     SND_SEQ_PORT_CAP_WRITE |
			     SND_SEQ_PORT_CAP_SUBS_WRITE,
			     SND_SEQ_PORT_TYPE_APPLICATION);
  return true;
}


void Sequencer::track_added(int track) {
  if (m_valid) {
    char track_name[10];
    sprintf(track_name, "Track %d", track);
    snd_seq_port_info_t* port_info;
    snd_seq_port_info_malloc(&port_info);
    snd_seq_port_info_set_port_specified(port_info, 1);
    snd_seq_port_info_set_port(port_info, track);
    snd_seq_port_info_set_name(port_info, track_name);
    snd_seq_port_info_set_capability(port_info, SND_SEQ_PORT_CAP_READ |
				     SND_SEQ_PORT_CAP_SUBS_READ);
    snd_seq_port_info_set_type(port_info, SND_SEQ_PORT_TYPE_APPLICATION);
    if (snd_seq_create_port(m_alsa_client, port_info))
      cerr<<"Could not create sequencer port for track "<<track<<"!"<<endl;
    snd_seq_port_info_free(port_info);
  }
}


void Sequencer::track_removed(int track) {
  if (m_valid)
    snd_seq_delete_simple_port(m_alsa_client, track);
}


void Sequencer::sequencing_loop() {
  cerr<<"sequencing_loop()"<<endl;
  while (m_valid) {
    jack_position_t pos;
    jack_transport_state_t state = jack_transport_query(m_jack_client, &pos);
    
    // need to sync
    if (m_sync_state == Syncing) {
      Mutex::Lock lock(m_song.get_big_lock());
      map<int, Track*>::const_iterator iter = m_song.get_tracks().begin();
      for ( ; iter != m_song.get_tracks().end(); ++iter)
	iter->second->find_next_note(pos.beat, pos.tick);
      m_sync_state = SyncDone;
    }

    // is it over yet?
    if (pos.bar * pos.beats_per_bar + pos.beat >= m_song.get_length()) {
      jack_transport_stop(m_jack_client);
      jack_position_t beginning;
      memset(&beginning, 0, sizeof(jack_position_t));
      jack_transport_reposition(m_jack_client, &beginning);
    }
      
    // record MIDI events
    int npfd = snd_seq_poll_descriptors_count(m_alsa_client, POLLIN);
    pollfd *pfd = (pollfd *)alloca(npfd* sizeof(pollfd));
    snd_seq_poll_descriptors(m_alsa_client, pfd, npfd, POLLIN);
    if (poll(pfd, npfd, 1) > 0) {
      snd_seq_event_t *ev;
      do {
	snd_seq_event_input(m_alsa_client, &ev);
	if (m_sync_state == InSync) {
	  // insert events into song here
	}
	snd_seq_free_event(ev);
      } while (snd_seq_event_input_pending(m_alsa_client, 0) > 0);
    }
    
    // play MIDI events
    else if ((state == JackTransportRolling) && m_sync_state == InSync){
      Mutex::Lock lock(m_song.get_big_lock());
      int beat, tick, value, length, client, port, channel;
      int tick_ahead = 10000;
      int tick_drop = 500;
      int current_beat = int(pos.bar * pos.beats_per_bar) + pos.beat;
      int before_beat = current_beat + (pos.tick + tick_ahead) / 10000;
      int before_tick = (pos.tick + tick_ahead) % 10000;
      map<int, Track*>::const_iterator iter = m_song.get_tracks().begin();
      for ( ; iter != m_song.get_tracks().end(); ++iter) {
	while(iter->second->get_next_note(beat, tick, value, length, 
					  before_beat, before_tick))
	  if ((current_beat - beat) * 10000 + pos.tick - tick <= tick_drop) {
	    schedule_note(beat, tick, iter->first, iter->second->get_channel(),
			  value, 64, length);
	  }
      }
      snd_seq_drain_output(m_alsa_client);
    }
    
  }
}


void Sequencer::schedule_note(int beat, int tick, int port, int channel, 
			      int value, int velocity, int length) {
  //cerr<<"schedule_note("<<beat<<", "<<tick<<", "<<port<<", "<<channel<<", "
  //    <<value<<", "<<velocity<<", "<<length<<")"<<endl;
  // calculate note start time and length in milliseconds
  jack_position_t pos;
  jack_transport_query(m_jack_client, &pos);
  int frame_offset = 
    jack_get_current_transport_frame(m_jack_client) - pos.frame;
  int current_beat = int(pos.bar * pos.beats_per_bar + pos.beat);
  double ticks_left = 
    (beat-current_beat) * pos.ticks_per_beat + tick - pos.tick;
  double ticks_per_ms = pos.beats_per_minute / 6;
  int ms_left = int(ticks_left / ticks_per_ms - 
		    frame_offset * 1000 / pos.frame_rate);
  int ms_length = int(length / ticks_per_ms);
  
  // make sure that the NOTE OFF happens before next NOTE ON for this tone
  ms_length -= 1;
  
  // create and schedule note event (ALSA generates the note on and note off)
  snd_seq_event_t ev;
  int err;
  snd_seq_ev_clear(&ev);
  snd_seq_ev_set_source(&ev, port);
  snd_seq_ev_set_subs(&ev);
  snd_seq_ev_set_note(&ev, channel, value, velocity, ms_length);
  int nanos = (ms_left * int(1e6)) % int(1e9);
  snd_seq_real_time ssrt = { ms_left / 1000, nanos > 0 ? nanos : 0 };
  snd_seq_ev_schedule_real(&ev, m_alsa_queue, 1, &ssrt);
  if ((err = snd_seq_event_output(m_alsa_client, &ev)) < 0)
    cerr<<snd_strerror(err)<<endl;
}


int Sequencer::jack_sync_callback(jack_transport_state_t state, 
				  jack_position_t* pos) {
  cerr<<"jack_sync_callback()"<<endl;
  if (m_sync_state == InSync || m_sync_state == Waiting) {
    cerr<<"set to Syncing"<<endl;
    m_sync_state = Syncing;
    return 0;
  }
  if (m_sync_state == SyncDone) {
    cerr<<"InSync"<<endl;
    m_sync_state = InSync;
    return 1;
  }
  return 0;
}


void Sequencer::jack_timebase_callback(jack_transport_state_t state, 
				       jack_nframes_t nframes, 
				       jack_position_t* pos, 
				       int new_pos) {
  double bpm = 150;
  int bpb = 4;
  double fpb = 44100 * 60 / bpm;
  double fpt = fpb / 10000;
  pos->bar = int(pos->frame / (bpb * fpb));
  pos->beat = int(pos->frame / fpb) % bpb;
  pos->tick = int(pos->frame / fpt ) % 10000;
  pos->beats_per_minute = bpm;
  pos->beats_per_bar = bpb;
  pos->ticks_per_beat = 10000;
  pos->valid = JackPositionBBT;
}
