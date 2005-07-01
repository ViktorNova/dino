/****************************************************************************
    
    midievent.hpp - A MIDI event implementation for the Dino sequencer
    
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

#ifndef MIDIEVENT_HPP
#define MIDIEVENT_HPP

#ifndef NULL
#define NULL 0
#endif


class MIDIEvent {
public:
  
  MIDIEvent(bool on, int stp, int val, int vel, int len, MIDIEvent* ass = NULL) 
    : m_step(stp), m_length(len), m_assoc(ass) {
    m_data[0] = (on ? 1 : 0);
    m_data[1] = val;
    m_data[2] = vel;
  }
  
  unsigned int get_length() const;
  unsigned int get_step() const;
  unsigned char get_type() const;
  unsigned char get_note() const;
  unsigned char get_velocity() const;
  MIDIEvent* get_next() const;
  MIDIEvent* get_previous() const;
  MIDIEvent* get_assoc() const;

  void set_length(unsigned int length);
  void set_step(unsigned int step);
  void set_type(unsigned char type);
  void set_note(unsigned char note);
  void set_velocity(unsigned char vel);
  void set_next(MIDIEvent* event);
  void set_previous(MIDIEvent* event);
  void set_assoc(MIDIEvent* event);
  
protected:
  unsigned int m_step;
  unsigned int m_length;
  MIDIEvent* m_next;
  MIDIEvent* m_previous;
  MIDIEvent* m_assoc;
  unsigned char m_data[4];
};


#endif
