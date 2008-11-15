/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#ifndef MIDIBUFFER_HPP
#define MIDIBUFFER_HPP

#include "songtime.hpp"


namespace Dino {
  
  
  /** This class is an output buffer that outgoing MIDI events should
      be added to. It is mostly a wrapper around a JACK MIDI output buffer. */
  class MIDIBuffer {
  public:
    
    /** Create a new MIDIBuffer as a wrapper for the JACK MIDI output buffer
	@c port_buffer. */
    MIDIBuffer(void* port_buffer, const SongTime& start_time, 
	       double bpm, unsigned long framerate);
    
    /** Set the JACK MIDI period size (this is needed when adding events to
	JACK MIDI buffers, not sure why). */
    void set_period_size(unsigned long nframes);
    
    /** Set the desired CC resolution in song time units. */
    void set_cc_resolution(const SongTime& resolution);
    
    /** Return the desired CC resolution in beats. Anyone who fills this buffer
	should try to add continuous controller events at least this close
	to each other. */
    const SongTime& get_cc_resolution() const;
    
    /** Set the current offset. This will be added to the timestamp of all 
	subsequently written events. */
    void set_offset(const SongTime& offset);
    
    /** Retrieves a buffer to write MIDI data to, or 0 if there is no 
	more space in the buffer. */
    unsigned char* reserve(const SongTime& time, size_t data_size);
    
    /** Copy MIDI data to the buffer. */
    int write(const SongTime& time, 
	      const unsigned char* data, size_t data_size);
    
  protected:
    
    /** The actual JACK MIDI output buffer. */
    void* m_buffer;
    
    /** The number of frames in this JACK cycle, needed when adding JACK MIDI
	events. */
    unsigned long m_nframes;
    
    /** The desired CC resolution. */
    SongTime m_cc_resolution;
    
    /** The song position at the beginning of the buffer. */
    SongTime m_start_beat;
    
    /** The BPM (constant). */
    double m_bpm;
    
    /** The sample rate. */
    unsigned long m_samplerate;
    
    SongTime m_offset;
    
  };


}


#endif
