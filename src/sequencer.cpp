#include <iostream>

#include <asoundlib.h>
#include <jack/jack.h>
#include <glibmm.h>

#include "sequencer.hpp"
#include "song.hpp"


using namespace Glib;
using namespace std;


Sequencer::Sequencer(const string& clientName) 
  : m_valid(false), m_song(NULL), m_sync_state(Syncing) {

  if (init_alsa(clientName) != 0)
    cerr<<"Could not initialise ALSA sequencer! "<<endl;
  else if (init_jack(clientName) != 0) 
    cerr<<"Could not initialise Jack! "<<endl;
  else
    m_valid = true;

  if (m_valid)
    signal_timeout().connect(sigc::mem_fun(*this, &Sequencer::check_sync), 10);
  else
    cerr<<"You will not be able to play any songs."<<endl;
}


Sequencer::~Sequencer() {
  
}
  

void Sequencer::set_song(const Song& song) {
  m_song = &song;
}


void Sequencer::play() {
  if (m_valid)
    jack_transport_start(m_jack_client);
}


void Sequencer::stop() {
  if (m_valid)
    jack_transport_stop(m_jack_client);
}


void Sequencer::goto_beat(double beat) {
  if (m_valid) {
    jack_position_t pos;
    memset(&pos, 0, sizeof(pos));
    jack_transport_reposition(m_jack_client, &pos);
  }
}


int Sequencer::init_alsa(const string& clientName) {
  int err;
  if ((err = snd_seq_open(&m_alsa_client, "default", SND_SEQ_OPEN_DUPLEX, 0)) !=0)
    return err;
  snd_seq_set_client_name(m_alsa_client, clientName.c_str());
  m_alsa_port = snd_seq_create_simple_port(m_alsa_client, "Track 0",
					SND_SEQ_PORT_CAP_READ | 
					SND_SEQ_PORT_CAP_SUBS_READ,
					SND_SEQ_PORT_TYPE_MIDI_GENERIC);
  m_alsa_queue = snd_seq_alloc_queue(m_alsa_client);
  snd_seq_start_queue(m_alsa_client, m_alsa_queue, NULL);

  return 0;
}


int Sequencer::init_jack(const string& clientName) {
  m_jack_client = jack_client_new(clientName.c_str());
  if (!m_jack_client)
    return -1;
  int err;

  if ((err = jack_set_process_callback(m_jack_client, 
				       &Sequencer::jack_process_callback_,
				       this)) != 0)
    return err;
  if ((err = jack_set_sync_callback(m_jack_client, &Sequencer::jack_sync_callback_,
				    this)) != 0)
    return err;
  if ((err = jack_set_timebase_callback(m_jack_client, 1, 
					&Sequencer::jack_timebase_callback_, 
					this)) != 0)
    return err;
  if ((err = jack_activate(m_jack_client)) != 0)
    return err;
  
  jack_position_t pos;
  memset(&pos, 0, sizeof(pos));
  jack_transport_stop(m_jack_client);
  jack_transport_reposition(m_jack_client, &pos);
  
  return 0;
}


bool Sequencer::check_sync() {
  if (m_sync_state == Syncing) {
    map<int, Track>::const_iterator iter;
    for (iter = m_song->get_tracks().begin(); iter != m_song->get_tracks().end();
	 ++iter)
      iter->second.find_next_note(m_sync_to_beat, m_sync_to_tick);
    m_sync_state = SyncDone;
  }
  return true;
}


int Sequencer::jack_sync_callback(jack_transport_state_t state, 
				jack_position_t* pos) {
  if (m_sync_state == InSync) {
    m_sync_to_beat = int(pos->bar * pos->beats_per_bar);
    m_sync_to_tick = pos->tick;
    m_sync_state = Syncing;
    return 0;
  }
  else if (m_sync_state == SyncDone) {
    m_sync_state = InSync;
    return 1;
  }
  return 0;
}


int Sequencer::jack_process_callback(jack_nframes_t nframes) {
  const int tickAhead = 100;
  
  // get the current beat and tick
  jack_position_t pos;
  jack_transport_state_t state = jack_transport_query(m_jack_client, &pos);
  const int cBeat = int(pos.bar * pos.beats_per_bar) + pos.beat;
  const int cTick = pos.tick;
  const int beforeBeat = cBeat + int((cTick + tickAhead) / 10000.0);
  const int beforeTick = (cTick + tickAhead) % 10000;
  
  // if we're rolling and we have something to sequence, sequence it
  if ((state == JackTransportRolling) && (pos.valid & JackPositionBBT) &&
      m_song) {
    const map<int, Track>& tracks(m_song->get_tracks());
    map<int, Track>::const_iterator iter;
    int beat, tick, value, length;
    for (iter = tracks.begin(); iter != tracks.end(); ++iter) {
      while (iter->second.get_next_note(beat, tick, value, length, 
					beforeBeat, beforeTick)) {
	if ((beat - cBeat) *  pos.ticks_per_beat + tick < cTick + tickAhead) {
	  schedule_note(beat, tick, value, length, cBeat, cTick);
	}
	else
	  break;
      }
    }
    snd_seq_drain_output(m_alsa_client);
  }
  
  return 0;
}


void Sequencer::jack_timebase_callback(jack_transport_state_t state,
				       jack_nframes_t nframes,
				       jack_position_t* pos, int new_pos) {
  pos->bar = pos->frame / (4*44100);
  pos->beat = (pos->frame / 44100) % 4;
  pos->tick = int(pos->frame / 4.41) % 10000;
  pos->beats_per_minute = 60;
  pos->beats_per_bar = 4;
  pos->ticks_per_beat = 10000;
  pos->valid = JackPositionBBT;
}


void Sequencer::schedule_note(int beat, int tick, int value, int length,
			      int currentBeat, int currentTick) {
  // calculate note length in milliseconds
  jack_position_t pos;
  jack_transport_query(m_jack_client, &pos);
  int realLength = int(length * 6.0 / (1.0 * pos.beats_per_minute));
  
  // create and schedule note event (ALSA generates the note on and note off)
  snd_seq_event_t ev;
  int err;
  snd_seq_ev_clear(&ev);
  snd_seq_ev_set_source(&ev, m_alsa_port);
  snd_seq_ev_set_subs(&ev);
  snd_seq_ev_set_note(&ev, 0, value, 64, realLength);
  snd_seq_real_time ssrt = { 0, 0 };
  snd_seq_ev_schedule_real(&ev, m_alsa_queue, 1, &ssrt);
  if ((err = snd_seq_event_output(m_alsa_client, &ev)) < 0)
    cerr<<snd_strerror(err)<<endl;
}
