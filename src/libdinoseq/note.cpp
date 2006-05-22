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
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include "note.hpp"
#include "noteevent.hpp"


namespace Dino {

  
  Note::Note(NoteEvent* on, NoteEvent* off)
    : m_note_on(on),
      m_note_off(off) {
    
  }

  
  unsigned int Note::get_length() const {
    return m_note_off->get_step() + 1 - m_note_on->get_step();
  }
  
  
  unsigned char Note::get_key() const {
    return m_note_on->get_key();
  }


  unsigned char Note::get_velocity() const {
    return m_note_on->get_velocity();
  }
  
  
  unsigned int Note::get_step() const {
    return m_note_on->get_step();
  }


  NoteEvent* Note::get_note_on() {
    return m_note_on;
  }
  
  
  NoteEvent* Note::get_note_off() {
    return m_note_off;
  }
  
  
}
