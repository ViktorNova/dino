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
  if (m_jack_client) {
    jack_transport_stop(m_jack_client);
    jack_position_t pos;
    memset(&pos, 0, sizeof(pos));
    jack_transport_reposition(m_jack_client, &pos);
    jack_client_close(m_jack_client);
  }
}
  

void Sequencer::set_song(const Song& song) {
  m_song = &song;
  m_song->signal_track_added.connect(mem_fun(*this, &Sequencer::track_added));
  m_song->signal_track_removed.
    connect(mem_fun(*this, &Sequencer::track_removed));
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


unsigned char Sequencer::get_alsa_id() const {
  snd_seq_client_info_t* seq_info;
  snd_seq_client_info_malloc(&seq_info);
  snd_seq_get_client_info(m_alsa_client, seq_info);
  int id = snd_seq_client_info_get_client(seq_info);
  snd_seq_client_info_free(seq_info);
  return id;
}


int Sequencer::init_alsa(const string& clientName) {
  int err;
  if ((err = snd_seq_open(&m_alsa_client, "default", SND_SEQ_OPEN_DUPLEX, 0))
      !=0)
    return err;
  snd_seq_set_client_name(m_alsa_client, clientName.c_str());
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


void Sequencer::track_added(int track) {
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
  else
    m_alsa_ports[track] = snd_seq_port_info_get_port(port_info);
  snd_seq_port_info_free(port_info);
}


void Sequencer::track_removed(int track) {
  snd_seq_delete_simple_port(m_alsa_client, m_alsa_ports[track]);
}


int Sequencer::jack_sync_callback(jack_transport_state_t state, 
				jack_position_t* pos) {
  if (m_sync_state == InSync || m_sync_state == Waiting) {
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

  // Nothing to sequence?
  if (!m_song)
    return 0;
  
  // try to get access to the song - if not, return (can't keep Jack waiting)
  Mutex::Lock lock(m_song->get_big_lock(), TRY_LOCK);
  if (!lock.locked())
    return 0;
  
  const int tickAhead = 100;
  
  // get the current beat and tick
  jack_position_t pos;
  jack_transport_state_t state = jack_transport_query(m_jack_client, &pos);
  const int cBeat = int(pos.bar * pos.beats_per_bar) + pos.beat;
  const int cTick = pos.tick;
  const int beforeBeat = cBeat + int((cTick + tickAhead) / 10000.0);
  const int beforeTick = (cTick + tickAhead) % 10000;
  
  // is it over yet?
  if (cBeat >= m_song->get_length()) {
    jack_transport_stop(m_jack_client);
    jack_position_t beginning;
    memset(&beginning, 0, sizeof(jack_position_t));
    jack_transport_reposition(m_jack_client, &beginning);
  }
  
  // if we're rolling and we have something to sequence, sequence it
  if ((state == JackTransportRolling) && (pos.valid & JackPositionBBT)) {
    const map<int, Track>& tracks(m_song->get_tracks());
    map<int, Track>::const_iterator iter;
    map<int, int>::const_iterator pIter = m_alsa_ports.begin();
    int beat, tick, value, length;
    for (iter = tracks.begin(); iter != tracks.end(); ++iter, ++pIter) {
      while (iter->second.get_next_note(beat, tick, value, length, 
					beforeBeat, beforeTick)) {
	if ((beat - cBeat) *  pos.ticks_per_beat + tick < cTick + tickAhead) {
	  schedule_note(beat, tick, value, length, 
			cBeat, cTick, pIter->second);
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
			      int currentBeat, int currentTick, int port) {
  // calculate note length in milliseconds
  jack_position_t pos;
  jack_transport_query(m_jack_client, &pos);
  int realLength = int(length * 6.0 / (1.0 * pos.beats_per_minute));
  
  // make sure that the NOTE OFF is before next NOTE ON
  realLength -= 1;
  
  // create and schedule note event (ALSA generates the note on and note off)
  snd_seq_event_t ev;
  int err;
  snd_seq_ev_clear(&ev);
  snd_seq_ev_set_source(&ev, port);
  snd_seq_ev_set_subs(&ev);
  snd_seq_ev_set_note(&ev, 0, value, 64, realLength);
  snd_seq_real_time ssrt = { 0, 0 };
  snd_seq_ev_schedule_real(&ev, m_alsa_queue, 1, &ssrt);
  if ((err = snd_seq_event_output(m_alsa_client, &ev)) < 0)
    cerr<<snd_strerror(err)<<endl;
}
