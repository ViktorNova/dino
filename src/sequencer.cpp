#include <iostream>

#include <asoundlib.h>
#include <jack/jack.h>
#include <glibmm.h>

#include "sequencer.hpp"
#include "song.hpp"


using namespace Glib;
using namespace std;


Sequencer::Sequencer(const string& clientName) 
  : mValid(false), mSong(NULL), mSyncState(Syncing) {

  if (initALSA(clientName) != 0)
    cerr<<"Could not initialise ALSA sequencer! "<<endl;
  else if (initJack(clientName) != 0) 
    cerr<<"Could not initialise Jack! "<<endl;
  else
    mValid = true;

  if (mValid)
    signal_timeout().connect(sigc::mem_fun(*this, &Sequencer::checkSync), 10);
  else
    cerr<<"You will not be able to play any songs."<<endl;
}


Sequencer::~Sequencer() {
  
}
  

void Sequencer::setSong(const Song& song) {
  mSong = &song;
}


void Sequencer::play() {
  if (mValid)
    jack_transport_start(jackClient);
}


void Sequencer::stop() {
  if (mValid)
    jack_transport_stop(jackClient);
}


void Sequencer::gotoBeat(double beat) {
  if (mValid) {
    jack_position_t pos;
    memset(&pos, 0, sizeof(pos));
    jack_transport_reposition(jackClient, &pos);
  }
}


int Sequencer::initALSA(const string& clientName) {
  int err;
  if ((err = snd_seq_open(&alsaClient, "default", SND_SEQ_OPEN_DUPLEX, 0)) !=0)
    return err;
  snd_seq_set_client_name(alsaClient, clientName.c_str());
  alsaPort = snd_seq_create_simple_port(alsaClient, "Track 0",
					SND_SEQ_PORT_CAP_READ | 
					SND_SEQ_PORT_CAP_SUBS_READ,
					SND_SEQ_PORT_TYPE_MIDI_GENERIC);
  alsaQueue = snd_seq_alloc_queue(alsaClient);
  snd_seq_start_queue(alsaClient, alsaQueue, NULL);

  return 0;
}


int Sequencer::initJack(const string& clientName) {
  jackClient = jack_client_new(clientName.c_str());
  if (!jackClient)
    return -1;
  int err;

  if ((err = jack_set_process_callback(jackClient, 
				       &Sequencer::jackProcessCallback_,
				       this)) != 0)
    return err;
  if ((err = jack_set_sync_callback(jackClient, &Sequencer::jackSyncCallback_,
				    this)) != 0)
    return err;
  if ((err = jack_set_timebase_callback(jackClient, 1, 
					&Sequencer::jackTimebaseCallback_, 
					this)) != 0)
    return err;
  if ((err = jack_activate(jackClient)) != 0)
    return err;
  
  jack_position_t pos;
  memset(&pos, 0, sizeof(pos));
  jack_transport_stop(jackClient);
  jack_transport_reposition(jackClient, &pos);
  
  return 0;
}


bool Sequencer::checkSync() {
  if (mSyncState == Syncing) {
    map<int, Track>::const_iterator iter;
    for (iter = mSong->getTracks().begin(); iter != mSong->getTracks().end();
	 ++iter)
      iter->second.findNextNote(mSyncToBeat, mSyncToTick);
    mSyncState = SyncDone;
  }
  return true;
}


int Sequencer::jackSyncCallback(jack_transport_state_t state, 
				jack_position_t* pos) {
  if (mSyncState == InSync) {
    mSyncToBeat = int(pos->bar * pos->beats_per_bar);
    mSyncToTick = pos->tick;
    mSyncState = Syncing;
    return 0;
  }
  else if (mSyncState == SyncDone) {
    mSyncState = InSync;
    return 1;
  }
  return 0;
}


int Sequencer::jackProcessCallback(jack_nframes_t nframes) {
  const int tickAhead = 100;
  
  // get the current beat and tick
  jack_position_t pos;
  jack_transport_state_t state = jack_transport_query(jackClient, &pos);
  const int cBeat = int(pos.bar * pos.beats_per_bar) + pos.beat;
  const int cTick = pos.tick;
  const int beforeBeat = cBeat + int((cTick + tickAhead) / 10000.0);
  const int beforeTick = (cTick + tickAhead) % 10000;
  
  // if we're rolling and we have something to sequence, sequence it
  if ((state == JackTransportRolling) && (pos.valid & JackPositionBBT) &&
      mSong) {
    const map<int, Track>& tracks(mSong->getTracks());
    map<int, Track>::const_iterator iter;
    int beat, tick, value, length;
    for (iter = tracks.begin(); iter != tracks.end(); ++iter) {
      while (iter->second.getNextNote(beat, tick, value, length, 
				      beforeBeat, beforeTick)) {
	if ((beat - cBeat) *  pos.ticks_per_beat + tick < cTick + tickAhead) {
	  scheduleNote(beat, tick, value, length, cBeat, cTick);
	}
	else
	  break;
      }
    }
    snd_seq_drain_output(alsaClient);
  }
  
  return 0;
}


void Sequencer::jackTimebaseCallback(jack_transport_state_t state,
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


void Sequencer::scheduleNote(int beat, int tick, int value, int length,
			     int currentBeat, int currentTick) {
  // calculate note length in milliseconds
  jack_position_t pos;
  jack_transport_query(jackClient, &pos);
  int realLength = int(length * 6.0 / (1.0 * pos.beats_per_minute));
  
  // create and schedule note event (ALSA generates the note on and note off)
  snd_seq_event_t ev;
  int err;
  snd_seq_ev_clear(&ev);
  snd_seq_ev_set_source(&ev, alsaPort);
  snd_seq_ev_set_subs(&ev);
  snd_seq_ev_set_note(&ev, 0, value, 64, realLength);
  snd_seq_real_time ssrt = { 0, 0 };
  snd_seq_ev_schedule_real(&ev, alsaQueue, 1, &ssrt);
  if ((err = snd_seq_event_output(alsaClient, &ev)) < 0)
    cerr<<snd_strerror(err)<<endl;
}
