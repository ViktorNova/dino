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


  /** This class represents a MIDI note event in a Pattern. It contains
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
    
    /** Return the length of the note in pattern steps. */
    unsigned int get_length() const;
    /** Return the pattern step where this event occurs. */
    unsigned int get_step() const;
    /** Return a pointer to the previous event at the same pattern step. */
    NoteEvent* get_previous() const;
    /** Return a pointer to the matching note on or note off event. */
    NoteEvent* get_assoc() const;
    
    /** Set the length of the note in pattern steps. */
    void set_length(unsigned int length);
    /** Set the pattern step where this event occurs. */
    void set_step(unsigned int step);
    /** Set the pointer to the previous event at the same pattern step. */
    void set_previous(NoteEvent* event);
    /** Set the pointer to the matching note on or note off event. */
    void set_assoc(NoteEvent* event);
  
  protected:
    
    /** The pattern step where this note event occurs. */
    unsigned int m_step;
    /** The length of this note in pattern steps. */
    unsigned int m_length;
    /** A pointer to the previous event at the same pattern step. */
    NoteEvent* m_previous;
    /** A pointer to the matching note on or note off event. */
    NoteEvent* m_assoc;
  };


}


#endif
