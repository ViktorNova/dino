/****************************************************************************
    
    basemidievent.hpp - A MIDI event implementation for the Dino sequencer
    
    Copyright (C) 2005  Lars Luthman <larsl@users.sourceforge.net>
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef BASEMIDIEVENT_HPP
#define BASEMIDIEVENT_HPP

#ifndef NULL
#define NULL 0
#endif

#include "deletable.hpp"


namespace Dino {


  /** This class represents a MIDI events in a Pattern. It contains
      information about the type of event and the data associated
      with the event, as well as where in the Pattern the event appears
      and other related information. */
  class BaseMIDIEvent : public Deletable {
  public:
  
    BaseMIDIEvent(unsigned char type, unsigned char data1, unsigned char data2) 
      : m_next(NULL) {
      m_data[0] = type;
      m_data[1] = data1;
      m_data[2] = data2;
    }
  
    unsigned char get_type() const;
    unsigned char get_note() const;
    unsigned char get_velocity() const;
    unsigned char get_value() const;
    BaseMIDIEvent* get_next() const;
    const unsigned char* get_data() const;
    unsigned int get_size() const;
    unsigned char get_channel() const;
  
    void set_type(unsigned char type);
    void set_note(unsigned char note);
    void set_velocity(unsigned char vel);
    void set_value(unsigned char value);
    void set_next(BaseMIDIEvent* event);
    void set_channel(unsigned char channel);
  
    static const unsigned char NoteOn = 0x90;
    static const unsigned char NoteOff = 0x80;
    static const unsigned char Controller = 0xB0;
    static const unsigned char PitchWheel = 0xE0;
  
    static BaseMIDIEvent AllNotesOff;
    static BaseMIDIEvent ChannelVolume;
    
  protected:

    BaseMIDIEvent* m_next;
    unsigned char m_data[4];
    
  };


}


#endif
