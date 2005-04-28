#include "jackclient.hpp"


JackClient::JackClient(string client_name) 
  : m_last_beat(0), m_last_tick(0), m_last_frame(0), m_tpb(10000) {
  int up = 0;
  
  m_jack_client = jack_client_new(client_name.c_str());
  if (!m_jack_client)
    throw up;
  
  int err;
  if ((err = jack_set_sync_callback(m_jack_client, 
				    &JackClient::jack_sync_callback_,
				    this)) != 0)
    throw up;
  if ((err=jack_set_timebase_callback(m_jack_client, 1, 
				      &JackClient::jack_timebase_callback_,
				      this)) != 0)
    throw up;
  if ((err = jack_activate(m_jack_client)) != 0)
    throw up;
  
  jack_position_t pos;
  memset(&pos, 0, sizeof(pos));
  jack_transport_stop(m_jack_client);
  jack_transport_reposition(m_jack_client, &pos);
}


void JackClient::transport_start() {
  jack_transport_start(m_jack_client);
}


void JackClient::transport_stop() {
  jack_transport_stop(m_jack_client);
}


void JackClient::transport_reposition(jack_position_t* pos) {
  jack_transport_reposition(m_jack_client, pos);
}


void JackClient::transport_locate(jack_nframes_t frame) {
  jack_transport_locate(m_jack_client, frame);
}


jack_transport_state_t JackClient::transport_query(jack_position_t* pos) {
  return jack_transport_query(m_jack_client, pos);
}


jack_nframes_t JackClient::get_current_transport_frame() {
  return jack_get_current_transport_frame(m_jack_client);
}


  
void JackClient::set_bpm(double bpm) {
  m_bpm = bpm;
}


double JackClient::get_bpm() const {
  return m_bpm;
}


void JackClient::set_bpb(int bpb) {
  m_bpb = bpb;
}


int JackClient::get_bpb() const {
  return m_bpb;
}


void JackClient::set_tpb(int tpb) {
  m_tpb = tpb;
}


int JackClient::get_tpb() const {
  return m_tpb;
}


jack_nframes_t JackClient::get_sample_rate() const {
  return jack_get_sample_rate(m_jack_client);
}


void JackClient::set_sync_state(SyncState state) {
  m_sync_state = state;
}


JackClient::SyncState JackClient::get_sync_state() const {
  return m_sync_state;
}


void JackClient::set_last_timebase(int last_beat, int last_tick, 
				   jack_nframes_t last_frame) {
  m_last_beat = last_beat;
  m_last_tick = last_tick;
  m_last_frame = last_frame;
}


int JackClient::jack_sync_callback(jack_transport_state_t state, 
				   jack_position_t* pos) {
  if (m_sync_state == InSync || m_sync_state == Waiting) {
    m_sync_state = Syncing;
    return 0;
  }
  if (m_sync_state == SyncDone) {
    m_sync_state = InSync;
    return 1;
  }
  return 0;
}


void JackClient::jack_timebase_callback(jack_transport_state_t state, 
					jack_nframes_t nframes, 
					jack_position_t* pos, 
					int new_pos) {
  if (m_sync_state == InSync) {
    double bpm = m_bpm;
    int bpb = 4;
    double fpb = pos->frame_rate * 60 / bpm;
    double fpt = fpb / m_tpb;
    int d_beat = int((pos->frame - m_last_frame) / fpb);
    int d_tick = int((pos->frame - m_last_frame) / fpt) % m_tpb;
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
}

