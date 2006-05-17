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

#ifndef MIDIBUFFER_HPP
#define MIDIBUFFER_HPP


namespace Dino {
  
  
  /** This class is an output buffer that outgoing MIDI events should
      be added to. It is mostly a wrapper around a JACK MIDI output buffer. */
  class MIDIBuffer {
  public:
    
    /** Create a new MIDIBuffer as a wrapper for the JACK MIDI output buffer
	@c port_buffer. */
    MIDIBuffer(void* port_buffer, double start_beat, 
	       double bpm, unsigned long framerate);
    
    /** Set the JACK MIDI period size (this is needed when adding events to
	JACK MIDI buffers, not sure why). */
    void set_period_size(unsigned long nframes);
    
    /** Set the desired CC resolution in beats. */
    void set_cc_resolution(double beats);
    
    /** Return the desired CC resolution in beats. Anyone who fills this buffer
	should try to add continuous controller events at least this close
	to each other. */
    double get_cc_resolution() const;
    
    /** Retrieves a buffer to write MIDI data to, or 0 if there is no more space
	in the buffer. */
    unsigned char* reserve(double beat, size_t data_size);
    
    /** Copy MIDI data to the buffer. */
    int write(double beat, const unsigned char* data, size_t data_size); 
    
  protected:
    
    /** The actual JACK MIDI output buffer. */
    void* m_buffer;
    
    /** The number of frames in this JACK cycle, needed when adding JACK MIDI
	events. */
    unsigned long m_nframes;
    
    /** The desired CC resolution. */
    double m_cc_resolution;
    
    /** The beat position at the beginning of the buffer. */
    double m_start_beat;
    
    /** The BPM (constant). */
    double m_bpm;
    
    /** The sample rate. */
    unsigned long m_samplerate;
    
  };


}


#endif
