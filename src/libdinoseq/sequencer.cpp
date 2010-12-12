/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <larsl@users.sourceforge.net>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>

#include <jack/midiport.h>
#include <glibmm.h>

#include "debug.hpp"
#include "deleter.hpp"
#include "sequencer.hpp"
#include "song.hpp"
#include "track.hpp"
#include "midibuffer.hpp"


namespace Dino {

  Sequencer::Sequencer(const string& client_name) 
    : m_client_name(client_name),
      m_song(0),
      m_valid(false),
      m_cc_resolution(0.01),
      m_time_to_next_cc(0),
      m_last_beat(0), 
      m_last_tick(0), 
      m_sent_all_off(false),
      m_current_beat(0), 
      m_old_current_beat(-1),
      m_ports_changed(0),
      m_old_ports_changed(0) {
  
    dbg1<<"Initialising sequencer"<<endl;
  
    if (!init_jack(m_client_name)) {
      dbg0<<"Could not initialise JACK!"<<endl;
      dbg0<<"Could not initialise sequencer!"<<endl;
      return;
    }

    m_valid = true;

    Glib::signal_timeout().connect(mem_fun(*this,&Sequencer::beat_checker), 20);
    Glib::signal_timeout().connect(mem_fun(*this,&Sequencer::ports_checker),20);
  }


  Sequencer::~Sequencer() {
    dbg1<<"Destroying sequencer"<<endl;
    if (m_valid) {
      stop();
      go_to_beat(0);
      m_valid = false;
      if (m_jack_client)
	jack_client_close(m_jack_client);
    }
  }


  bool Sequencer::set_song(Song& song) {
    if (m_song)
      return false;
    m_song = &song;
    m_song->signal_track_added.connect(mem_fun(*this, &Sequencer::track_added));
    m_song->signal_track_removed.
      connect(mem_fun(*this, &Sequencer::track_removed));
    reset_ports();
    return true;
  }


  unsigned long Sequencer::get_frame_rate() {
    if (!m_jack_client)
      return 48000;
    return jack_get_sample_rate(m_jack_client);
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
      jack_transport_locate(m_jack_client, m_song->bt2frame(beat));
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
    
      const char** ports = jack_get_ports(m_jack_client, 0, 
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
	dbg0<<"Trying to connect nonexistant output port "<<track
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
      if (instrument != "None") {
	jack_connect(m_jack_client, 
		     jack_port_name(iter->second), instrument.c_str());
      }
    }
  }


  void Sequencer::reset_ports() {
    Song::ConstTrackIterator iter;
    for (iter = m_song->tracks_begin(); iter != m_song->tracks_end(); ++iter)
      track_added(iter->get_id());
  }

  
  bool Sequencer::init_jack(const string& client_name) {
  
    dbg1<<"Initialising JACK client"<<endl;
  
    jack_set_error_function(&Sequencer::jack_error_function);
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
    if ((err = jack_set_port_registration_callback(m_jack_client,
						   &Sequencer::jack_port_registration_callback_,
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
      std::sprintf(track_name, "Track %d", track);
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
    pos->beats_per_bar = 4;
    pos->ticks_per_beat = 10000;
    
    
    // can't pass references to pos-> members directly since it's packed
    int32_t beat, tick;
    double bpm, frame_offset;
    m_song->get_timebase_info(pos->frame, pos->frame_rate, pos->ticks_per_beat,
			     bpm, beat, tick, frame_offset);
    pos->beats_per_minute = bpm;
    pos->bar_start_tick = frame_offset;

    // if we are standing still or if we just relocated, calculate 
    // the new position
    if (new_pos || state != JackTransportRolling) {
      pos->beat = beat;
      pos->tick = tick;
    }
    // otherwise, just increase the BBT by a period
    else {
      double db = nframes * pos->beats_per_minute / (pos->frame_rate * 60.0);
      pos->beat = m_last_beat + int32_t(db);
      pos->tick = m_last_tick + int32_t((db - int(db)) * pos->ticks_per_beat);
      if (pos->tick >= pos->ticks_per_beat) {
	pos->tick -= int32_t(pos->ticks_per_beat);
	++pos->beat;
      }
    }
  
    m_last_beat = pos->beat;
    m_last_tick = pos->tick;

    pos->bar = int32_t(pos->beat / pos->beats_per_bar);
    pos->beat %= int(pos->beats_per_bar);
    pos->valid = JackPositionBBT;
    
    // bars and beats start from 1 by convention (but ticks don't!)
    ++pos->bar;
    ++pos->beat;
  }


  int Sequencer::jack_process_callback(jack_nframes_t nframes) {
    jack_position_t pos;
    jack_transport_state_t state = jack_transport_query(m_jack_client, &pos);
    --pos.bar;
    --pos.beat;
  
    // first, tell the GUI thread that it's OK to delete unused objects
    Deleter::get_instance().confirm();
  
    // no valid time info, don't do anything
    if (!(pos.valid & JackTransportBBT))
      return 0;
  
    // set the current beat
    m_current_beat = pos.bar * int(pos.beats_per_bar) + pos.beat;
    
    // at the end of the song, stop and go back to the beginning
    if (m_current_beat >= m_song->get_length()) {
      jack_transport_stop(m_jack_client);
      jack_transport_locate(m_jack_client, 0);
      return 0;
    }
  
    sequence_midi(state, pos, nframes);
  
    return 0;
  }


  void Sequencer::jack_shutdown_handler() {
    // XXX do something useful here
    dbg0<<"JACK has shut down!"<<endl;
  }
  

  void Sequencer::jack_port_registration_callback(jack_port_id_t port, int m) {
    if (m == 0)
      m_ports_changed = m_ports_changed + 1;
    else {
      int flags = jack_port_flags(jack_port_by_id(m_jack_client, port));
      const char* type = jack_port_type(jack_port_by_id(m_jack_client, port));
      if ((flags & JackPortIsInput) && !strcmp(type, JACK_DEFAULT_MIDI_TYPE))
	m_ports_changed = m_ports_changed + 1;
    }
  }


  void Sequencer::sequence_midi(jack_transport_state_t state, 
				const jack_position_t& pos, 
				jack_nframes_t nframes) {
    
    // if we're not rolling, turn off all notes and return
    Song::ConstTrackIterator iter;
    if (state != JackTransportRolling) {
      for (iter = m_song->tracks_begin(); iter != m_song->tracks_end(); ++iter) {
	jack_port_t* port = m_output_ports[iter->get_id()];
	if (port) {
	  void* port_buf = jack_port_get_buffer(port, nframes);
	  jack_midi_clear_buffer(port_buf);
	  unsigned char all_notes_off[] = { 0xB0 | (iter->get_channel() & 0xF),
					    123, 0 };
	  if (!m_sent_all_off)
	    jack_midi_event_write(port_buf, 0, all_notes_off, 3);
	}
      }
      m_sent_all_off = true;
      return;
    }
    m_sent_all_off = false;
    
    // if we are rolling, sequence MIDI
    /* can't abuse this field, some transport masters actually set it
       to bar * bpm * tpb (what's the point in that?) */
    /*double offset = pos.bar_start_tick * pos.beats_per_minute / 
      (pos.frame_rate * 60);*/
    double offset = 0;
    double start = pos.bar * pos.beats_per_bar + pos.beat + 
      pos.tick / double(pos.ticks_per_beat) + offset;
    double end = start + pos.beats_per_minute * nframes / 
      (60 * pos.frame_rate) + offset;
    for (iter = m_song->tracks_begin(); iter != m_song->tracks_end(); ++iter) {

      // get the MIDI buffer
      jack_port_t* port = m_output_ports[iter->get_id()];
      if (port) {
	void* port_buf = jack_port_get_buffer(port, nframes);
	jack_midi_clear_buffer(port_buf);
	MIDIBuffer buffer(port_buf, start, pos.beats_per_minute,pos.frame_rate);
	buffer.set_period_size(nframes);
	buffer.set_cc_resolution(m_cc_resolution * pos.beats_per_minute / 60);
	iter->sequence(buffer, start, end);
      }
    }

  }


  bool Sequencer::beat_checker() {
    int current_beat = m_current_beat;
    if (current_beat != m_old_current_beat) {
      m_old_current_beat = current_beat;
      signal_beat_changed(const_cast<int&>(m_current_beat));
    }
    return true;
  }
  
  
  bool Sequencer::ports_checker() {
    int ports_changed = m_ports_changed;
    if (ports_changed != m_old_ports_changed) {
      m_old_ports_changed = ports_changed;
      signal_instruments_changed();
    }
    return true;
  }


}
