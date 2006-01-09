/****************************************************************************
    
    noteevent.hpp - A MIDI event implementation for the Dino sequencer
    
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

#ifndef NOTEEVENT_HPP
#define NOTEEVENT_HPP

#ifndef NULL
#define NULL 0
#endif

#include "midievent.hpp"


namespace Dino {


  /** This class represents a MIDI events in a Pattern. It contains
      information about the type of event and the data associated
      with the event, as well as where in the Pattern the event appears
      and other related information. */
  class NoteEvent : public MIDIEvent {
  public:
  
    NoteEvent(unsigned char type, int stp, int val, 
	      int vel, int len, NoteEvent* ass = NULL) 
      : MIDIEvent(type, val, vel),
	m_step(stp), m_length(len), m_previous(NULL), m_assoc(ass) {
    
    }
  
    NoteEvent(unsigned char type, int stp, unsigned char data1, 
	      unsigned char data2) 
      : MIDIEvent(type, data1, data2),
	m_step(stp), m_length(0), m_previous(NULL), m_assoc(NULL) {

    }
  
    unsigned int get_length() const;
    unsigned int get_step() const;
    NoteEvent* get_previous() const;
    NoteEvent* get_assoc() const;
  
    void set_length(unsigned int length);
    void set_step(unsigned int step);
    void set_previous(NoteEvent* event);
    void set_assoc(NoteEvent* event);
  
  protected:

    unsigned int m_step;
    unsigned int m_length;
    NoteEvent* m_previous;
    NoteEvent* m_assoc;
  };


}


#endif
