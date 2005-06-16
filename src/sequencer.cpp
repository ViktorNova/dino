#include <iostream>

#include <glibmm.h>
extern "C" {
#include <jack/midiport.h>
}

#include "deleter.hpp"
#include "sequencer.hpp"
#include "song.hpp"


Sequencer::Sequencer(const string& client_name, Song& song) 
  : m_client_name(client_name), m_song(song), m_valid(false), 
    m_sync_state(Waiting) {
  
  if (!init_jack(m_client_name)) {
    cerr<<"Could not initialise JACK!"<<endl;
    return;
  }

  m_valid = true;

  m_song.signal_track_added.connect(mem_fun(*this, &Sequencer::track_added));
  m_song.signal_track_removed.
    connect(mem_fun(*this, &Sequencer::track_removed));
  
}


Sequencer::~Sequencer() {
  if (m_valid) {
    stop();
    go_to_beat(0);
    m_valid = false;
  }
  // XXX What the hell? delete m_jack_client;
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
    double second = m_song.get_second(int(beat), 
				      int((beat - int(beat)) * 10000));
    //m_jack_client->set_timebase_enabled(false);
    jack_transport_locate(m_jack_client,
			  int(second * jack_get_sample_rate(m_jack_client)));
  }
}
  
 
bool Sequencer::is_valid() const {
  return m_valid;
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
  if ((err = jack_set_process_callback(m_jack_client,
				       &Sequencer::jack_process_callback_,
				       this)) != 0)
    return false;
  if ((err = jack_activate(m_jack_client)) != 0)
    return false;
  
  jack_position_t pos;
  memset(&pos, 0, sizeof(pos));
  jack_transport_stop(m_jack_client);
  jack_transport_reposition(m_jack_client, &pos);
  m_input_port = jack_port_register(m_jack_client, "MIDI input", 
				    JACK_DEFAULT_MIDI_TYPE, 
				    JackPortIsInput, 0);
  return true;
}


void Sequencer::track_added(int track) {
  if (m_valid) {
    char track_name[10];
    sprintf(track_name, "Track %d", track);
    jack_port_t* port = jack_port_register(m_jack_client, track_name, 
					   JACK_DEFAULT_MIDI_TYPE, 
					   JackPortIsOutput, 0);
    m_output_ports[track] = port;
  }
}


void Sequencer::track_removed(int track) {
  if (m_valid) {
    jack_port_unregister(m_jack_client, m_output_ports[track]);
    m_output_ports.erase(track);
  }
}


int Sequencer::jack_sync_callback(jack_transport_state_t state, 
				   jack_position_t* pos) {
  return 1;
}


void Sequencer::jack_timebase_callback(jack_transport_state_t state, 
					jack_nframes_t nframes, 
					jack_position_t* pos, 
					int new_pos) {
  // XXX Horrible, horrible!
  // Need complete rewrite
  /*
  if (m_sync_state == InSync) {
    double bpm = m_bpm;
    int bpb = 4;
    double fpb = pos->frame_rate * 60 / bpm;
    double fpt = fpb / m_tpb;
    int d_frame = int(pos->frame) - int(m_last_frame);
    d_frame = d_frame > 0 ? d_frame : 0;
    int d_beat = int(d_frame / fpb);
    int d_tick = int(d_frame / fpt) % m_tpb;
    int current_beat = m_last_beat + d_beat + (m_last_tick + d_tick) / m_tpb;
    int current_tick = (m_last_tick + d_tick) % m_tpb;
    
    pos->bar = int(current_beat / bpb);
    pos->beat = int(current_beat % bpb);
    pos->tick = current_tick;
    pos->beats_per_minute = bpm;
    pos->beats_per_bar = bpb;
    pos->ticks_per_beat = m_tpb;
    pos->valid = JackPositionBBT;
    
    m_last_beat = current_beat;
    m_last_tick = current_tick;
    m_last_frame = pos->frame;  
  }
  */
  double bpm = 100;
  int ticks_per_beat = 10000;
  int beats_per_bar = 4;
  double beat = 100 * pos->frame / (pos->frame_rate * 60);
  pos->beats_per_bar = beats_per_bar;
  pos->beats_per_minute = bpm;
  pos->ticks_per_beat = ticks_per_beat;
  pos->bar = int(beat) / beats_per_bar;
  pos->beat = int(beat) % beats_per_bar;
  pos->tick = int(10000 * (beat - int(beat)));
  pos->valid = JackPositionBBT;
}


int Sequencer::jack_process_callback(jack_nframes_t nframes) {
  jack_position_t pos;
  jack_transport_state_t state = jack_transport_query(m_jack_client, &pos);
  
  // first, tell the GUI thread that it's OK to delete unused objects
  Pattern::NoteEvent* event;
  for (int i = 0; i < 100 && g_notes_not_used.pop(event); ++i) 
    g_notes_to_delete.push(event);
  
  // no valid time info, don't do anything
  if (!(pos.valid & JackTransportBBT))
    return 0;
  
  // at the end of the song, stop and go back to the beginning
  if (pos.bar * pos.beats_per_bar + pos.beat >= m_song.get_length()) {
    jack_transport_stop(m_jack_client);
    jack_transport_locate(m_jack_client, 0);
    return 0;
  }
  
  sequence_midi(state, pos, nframes);
  
  return 0;
}


void Sequencer::sequence_midi(jack_transport_state_t state, 
			      const jack_position_t& pos, 
			      jack_nframes_t nframes) {
  // if we're not rolling, turn off all notes and return
  if (state != JackTransportRolling) {
    map<int, Track*>::const_iterator iter = m_song.get_tracks().begin();
    for ( ; iter != m_song.get_tracks().end(); ++iter) {
      jack_port_t* port = m_output_ports[iter->first];
      void* port_buf = jack_port_get_buffer(port, nframes);
      jack_midi_clear_buffer(port_buf, nframes);
      unsigned char* p = 
	jack_midi_write_next_event(port_buf, 0, 3, nframes);
      if (p) {
	p[0] = 0xB0;
	p[1] = 0x7B;
	p[2] = 0x00;
      }
    }
    return;
  }
  
  
}
