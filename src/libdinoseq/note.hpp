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

#ifndef NOTE_HPP
#define NOTE_HPP


namespace Dino {
  
  
  class NoteEvent;
  class Pattern;
  
  /** A note. */
  class Note {
  public:
    
    /** Returns the length of the note in pattern steps. */
    unsigned int get_length() const;
    /** Returns the MIDI key of the note. */
    unsigned char get_key() const;
    /** Returns the MIDI velocity of the note. */
    unsigned char get_velocity() const;
    /** Returns the pattern step that the note starts on. */
    unsigned int get_step() const;
    /** Returns the note on event. */
    NoteEvent* get_note_on();
    /** Returns the note off event. */
    NoteEvent* get_note_off();
    
  protected:
    
    friend class Pattern;
    
    /** This is protected so only friends can create Note objects. */
    Note(NoteEvent* on, NoteEvent* off);
    
    NoteEvent* m_note_on;
    NoteEvent* m_note_off;
    
  };
  

}

#endif
