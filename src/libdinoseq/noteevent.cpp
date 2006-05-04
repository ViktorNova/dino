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

#include "noteevent.hpp"


using namespace std;


namespace Dino {


  NoteEvent::NoteEvent(unsigned char type, int stp, int val, int vel) 
    : m_step(stp), 
      m_previous(0),
      m_next(0),
      m_note(0) {
    assert(val >= 0);
    assert(vel >= 0);
    assert(val < 128);
    assert(vel < 128);
    m_data[0] = type;
    m_data[1] = val;
    m_data[2] = vel;
  }
  
  
  unsigned char NoteEvent::get_key() const {
    return m_data[1];
  }

  
  unsigned char NoteEvent::get_velocity() const {
    return m_data[2];
  }
  

  unsigned int NoteEvent::get_step() const {
    return m_step;
  }


  NoteEvent* NoteEvent::get_previous() const {
    return m_previous;
  }


  NoteEvent* NoteEvent::get_next() const {
    return m_next;
  }
  

  Note* NoteEvent::get_note() {
    return m_note;
  }


  const unsigned char* NoteEvent::get_data() const {
    return m_data;
  }

  
  void NoteEvent::set_key(unsigned char note) {
    m_data[1] = note;
  }

  
  void NoteEvent::set_velocity(unsigned char vel) {
    m_data[2] = vel;
  }
  

  void NoteEvent::set_step(unsigned int step) {
    m_step = step; 
  }


  void NoteEvent::set_previous(NoteEvent* event) {
    m_previous = event;
  }


  void NoteEvent::set_note(Note* note) {
    m_note = note;
  }


  void NoteEvent::set_next(NoteEvent* next) {
    m_next = next;
  }

  
}
