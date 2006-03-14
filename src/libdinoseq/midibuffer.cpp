#include <jack/midiport.h>

#include "midibuffer.hpp"


namespace Dino {

    
  MIDIBuffer::MIDIBuffer(void* port_buffer) 
    : m_buffer(port_buffer) { 
  
  }
    
  
  void MIDIBuffer::set_period_size(unsigned long nframes) {
    m_nframes = nframes;
  }
  
  
  unsigned char* MIDIBuffer::reserve(double beat, size_t data_size) {
    // XXX compute timestamp here
    return jack_midi_event_reserve(m_buffer, 0, data_size, m_nframes);
  }
    
  
  int MIDIBuffer::write(double beat, unsigned char* data, size_t data_size) {
    // XXX compute timestamp here
    return jack_midi_event_write(m_buffer, 0, data, data_size, m_nframes);
  }


}
