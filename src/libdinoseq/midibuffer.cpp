#include <jack/midiport.h>

#include "midibuffer.hpp"


namespace Dino {

    
  MIDIBuffer::MIDIBuffer(void* port_buffer) 
    : m_buffer(port_buffer) { 
  
  }
    
  
  void MIDIBuffer::set_period_size(unsigned long nframes) {
    m_nframes = nframes;
  }


  void MIDIBuffer::set_cc_resolution(double beats) {
    m_cc_resolution = beats;
  }


  double MIDIBuffer::get_cc_resolution() const {
    return m_cc_resolution;
  }
  
  
  unsigned char* MIDIBuffer::reserve(double beat, size_t data_size) {
    // XXX compute timestamp here
    return jack_midi_event_reserve(m_buffer, 0, data_size, m_nframes);
  }
    
  
  int MIDIBuffer::write(double beat, const 
			unsigned char* data, size_t data_size) {
    // XXX compute timestamp here
    return jack_midi_event_write(m_buffer, 0, (jack_midi_data_t*)data, 
				 data_size, m_nframes);
  }


}
