#include <iostream>

#include "sequencer.hpp"



Sequencer::Sequencer(const string& client_name, Song& song) 
  : m_client_name(client_name), m_song(song), m_valid(false) {

  if (!init_jack(m_client_name)) {
    cerr<<"Could not initialise JACK!"<<endl;
    return;
  }
  if (!init_alsa(m_client_name)) {
    cerr<<"Could not initialise ALSA!"<<endl;
    return;
  }
  m_valid = true;
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


void Sequencer::sequencing_loop() {
  while (m_valid) {
    
    // record MIDI events (the poll introduces a possible context switch)
    int npfd = snd_seq_poll_descriptors_count(m_alsa_client, POLLIN);
    pollfd *pfd = (pollfd *)alloca(npfd* sizeof(pollfd));
    snd_seq_poll_descriptors(m_alsa_client, pfd, npfd, POLLIN);
    if (poll(pfd, npfd, 1) > 0) {
      snd_seq_event_t *ev;
      do {
	snd_seq_event_input(m_alsa_client, &ev);
	// insert events into song here
	snd_seq_free_event(ev);
      } while (snd_seq_event_input_pending(m_alsa_client, 0) > 0);
    }
    
    // push events onto the queue
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
  pos->bar = pos->frame / (4*44100);
  pos->beat = (pos->frame / 44100) % 4;
  pos->tick = int(pos->frame / 4.41) % 10000;
  pos->beats_per_minute = 60;
  pos->beats_per_bar = 4;
  pos->ticks_per_beat = 10000;
  pos->valid = JackPositionBBT;
}
