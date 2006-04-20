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

#include <iostream>

#include "midievent.hpp"


using namespace std;


namespace Dino {


  MIDIEvent MIDIEvent::AllNotesOff(0xB0, 123, 0);
  MIDIEvent MIDIEvent::ChannelVolume(0xB0, 7, 10);

  
  unsigned char MIDIEvent::get_type() const {
    const unsigned char& sb = m_data[0];
    if (sb >= 0x80 && sb < 0xF0)
      return m_data[0] & 0xF0;
    return m_data[0];
  }


  MIDIEvent* MIDIEvent::get_next() const {
    return m_next;
  }


  const unsigned char* MIDIEvent::get_data() const {
    return m_data;
  }


  unsigned int MIDIEvent::get_size() const {
    return 3;
  }


  void MIDIEvent::set_type(unsigned char type) {
    m_data[0] = type;
  }


  void MIDIEvent::set_next(MIDIEvent* event) {
    m_next = event;
  }


}
