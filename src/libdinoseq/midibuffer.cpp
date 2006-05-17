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

#include <jack/midiport.h>

#include "midibuffer.hpp"


namespace Dino {

    
  MIDIBuffer::MIDIBuffer(void* port_buffer, double start_beat, double bpm,
			 unsigned long framerate) 
    : m_buffer(port_buffer),
      m_start_beat(start_beat),
      m_bpm(bpm),
      m_samplerate(framerate) { 
  
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
    // XXX optimise this
    jack_nframes_t timestamp = jack_nframes_t((beat - m_start_beat) * 
					      60 / (m_bpm * m_samplerate));
    return jack_midi_event_reserve(m_buffer, timestamp, data_size, m_nframes);
  }
    
  
  int MIDIBuffer::write(double beat, const 
			unsigned char* data, size_t data_size) {
    // XXX optimise this
    jack_nframes_t timestamp = jack_nframes_t((beat - m_start_beat) * 
					      60 / (m_bpm * m_samplerate));
    return jack_midi_event_write(m_buffer, timestamp, (jack_midi_data_t*)data, 
				 data_size, m_nframes);
  }


}
