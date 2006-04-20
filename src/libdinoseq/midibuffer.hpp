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
  
  
  class MIDIBuffer {
  public:
    
    MIDIBuffer(void* port_buffer);
    
    void set_period_size(unsigned long nframes);
    
    void set_cc_resolution(double beats);
    
    double get_cc_resolution() const;
    
    unsigned char* reserve(double beat, size_t data_size);
    
    int write(double beat, const unsigned char* data, size_t data_size); 
    
  protected:
    
    void* m_buffer;
    unsigned long m_nframes;
    double m_cc_resolution;
  };


}


#endif
