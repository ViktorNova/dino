#include <iostream>

extern "C" {
#include <jack/midiport.h>
}

#include "deleter.hpp"
#include "sequencer.hpp"
#include "song.hpp"


Sequencer::Sequencer(const string& client_name, Song& song) 
  : m_client_name(client_name), m_song(song), m_valid(false) {
  
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
    jack_transport_locate(m_jack_client,
			  int(second * jack_get_sample_rate(m_jack_client)));
  }
}
  
 
bool Sequencer::is_valid() const {
  return m_valid;
}


vector<Sequencer::InstrumentInfo> Sequencer::get_instruments(int track) const {
  vector<InstrumentInfo> instruments;
  if (m_jack_client) {
    
    // check if the given track is connected to a port
    string connected_instrument;
    if (track != -1) {
      map<int, jack_port_t*>::const_iterator iter = m_output_ports.find(track);
      assert(iter != m_output_ports.end());
      const char** connected = jack_port_get_connections(iter->second);
      if (connected && connected[0])
	connected_instrument = connected[0];
      free(connected);
    }
    
    const char** ports = jack_get_ports(m_jack_client, NULL, 
					JACK_DEFAULT_MIDI_TYPE, 
					JackPortIsInput);
    if (ports) {
      for (size_t i = 0; ports[i]; ++i) {
	InstrumentInfo ii = ports[i];
	if (connected_instrument == ports[i])
	  ii.connected = true;
	instruments.push_back(ii);
      }
      free(ports);
    }
  }
  return instruments;
}
 

void Sequencer::set_instrument(int track, const string& instrument) {
  if (m_jack_client) {
    
    map<int, jack_port_t*>::const_iterator iter = m_output_ports.find(track);
    if (iter == m_output_ports.end()) {
      cerr<<"Trying to connect nonexistant output port "<<track
	  <<" to instrument "<<instrument<<endl;
      return;
    }

    // remove old connections from this port
    const char** ports = jack_port_get_connections(iter->second);
    if (ports) {
      for (size_t i = 0; ports[i]; ++i)
	jack_disconnect(m_jack_client, jack_port_name(iter->second), ports[i]);
      free(ports);
    }
    
    // connect the new instrument
    jack_connect(m_jack_client, 
		 jack_port_name(iter->second), instrument.c_str());
  }
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
  if ((err = jack_set_timebase_callback(m_jack_client, 1, 
					&Sequencer::jack_timebase_callback_,
					this)) != 0)
    return false;
  if ((err = jack_set_process_callback(m_jack_client,
				       &Sequencer::jack_process_callback_,
				       this)) != 0)
    return false;

  jack_on_shutdown(m_jack_client, &Sequencer::jack_shutdown_handler_, this);
  
  if ((err = jack_activate(m_jack_client)) != 0)
    return false;
  
  jack_position_t pos;
  memset(&pos, 0, sizeof(pos));
  jack_transport_stop(m_jack_client);
  jack_transport_reposition(m_jack_client, &pos);
  /*
  m_input_port = jack_port_register(m_jack_client, "MIDI input", 
				    JACK_DEFAULT_MIDI_TYPE, 
				    JackPortIsInput, 0);
  */
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


void Sequencer::jack_timebase_callback(jack_transport_state_t state, 
				       jack_nframes_t nframes, 
				       jack_position_t* pos, 
				       int new_pos) {
  int32_t beat, tick;
  m_song.get_timebase_info(pos->frame, pos->frame_rate, pos->beats_per_minute, 
			   beat, tick);

  pos->beats_per_bar = 4;
  pos->ticks_per_beat = 10000;
  
  if (new_pos || state != JackTransportRolling) {
    pos->beat = beat;
    pos->tick = tick;
    cerr<<"New position!"<<endl;
  }
  else {
    double db = nframes * pos->beats_per_minute / (pos->frame_rate * 60.0);
    pos->beat += int32_t(db);
    pos->tick += int32_t((db - int(db)) * pos->ticks_per_beat);
    if (pos->tick >= pos->ticks_per_beat) {
      pos->tick -= int32_t(pos->ticks_per_beat);
      ++pos->beat;
    }
  }
  
  pos->bar = int32_t(pos->beat / pos->beats_per_bar);
  pos->valid = JackPositionBBT;
}


int Sequencer::jack_process_callback(jack_nframes_t nframes) {
  jack_position_t pos;
  jack_transport_state_t state = jack_transport_query(m_jack_client, &pos);
  
  // first, tell the GUI thread that it's OK to delete unused objects
  MIDIEvent* event;
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


void Sequencer::jack_shutdown_handler() {
  cerr<<"JACK SHUT DOWN!"<<endl;
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
      unsigned char* p = jack_midi_write_next_event(port_buf, 0, 3, nframes);
      if (p) {
	p[0] = 0xB0;
	p[1] = 0x7B;
	p[2] = 0x00;
      }
    }
    return;
  }
  
  // if we are rolling, sequence MIDI
  unsigned int beat = pos.beat, tick = pos.tick;
  unsigned int ticks = (unsigned int)
    (nframes * pos.beats_per_minute * pos.ticks_per_beat / 
     (pos.frame_rate * 60.0));
  unsigned int list;
  unsigned int last_tick = (tick + ticks) % int(pos.ticks_per_beat);
  unsigned int last_beat = beat + (tick + ticks) / int(pos.ticks_per_beat);
  map<int, Track*>::const_iterator iter = m_song.get_tracks().begin();
  for ( ; iter != m_song.get_tracks().end(); ++iter) {
    
    // clear the MIDI buffer
    jack_port_t* port = m_output_ports[iter->first];
    void* port_buf = jack_port_get_buffer(port, nframes);
    jack_midi_clear_buffer(port_buf, nframes);
    
    // add events in buffer
    const Track* trk = iter->second;
    MIDIEvent* event;
    bool full = false;
    list = 0;
    while (!full &&
	   (event = trk->get_events(beat, tick, last_beat, last_tick, 
				    (unsigned int)(pos.ticks_per_beat),
				    list))) {
      for ( ; event; event = event->get_next())
	full = !add_event_to_buffer(event, port_buf, beat, tick, pos, nframes);
    }

  }
  
  
  
}


bool Sequencer::add_event_to_buffer(MIDIEvent* event, void* port_buf,
				    unsigned int beat, unsigned int tick,
				    const jack_position_t& pos, 
				    jack_nframes_t nframes) {
  cerr<<"NOTE "<<(event->get_type() == MIDIEvent::NoteOn ? "ON" : "OFF")
      <<" at "<<beat<<", "<<tick<<":"
      <<"\t"<<int(event->get_note())
      <<"\t"<<int(event->get_velocity())
      <<endl;
  double dt = (beat - pos.beat) * pos.ticks_per_beat + tick - pos.tick;
  jack_nframes_t frame = 
    jack_nframes_t(dt * 60 * pos.frame_rate / 
		   (pos.ticks_per_beat * pos.beats_per_minute));
  unsigned char* p = jack_midi_write_next_event(port_buf, frame, 3, nframes);
  if (p) {
    p[0] = event->get_type();
    p[1] = event->get_note();
    p[2] = event->get_velocity();
    return true;
  }
  else
    return false;
}
