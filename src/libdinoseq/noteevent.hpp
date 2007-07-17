/****************************************************************************
    
    noteevent.hpp - A MIDI event implementation for the Dino sequencer
    
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
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef NOTEEVENT_HPP
#define NOTEEVENT_HPP


namespace Dino {
  
  
  class Note;
  

  /** This class represents a MIDI note event in a Pattern. It contains
      information about the type of event and the data associated
      with the event, as well as where in the Pattern the event appears
      and other related information. */
  class NoteEvent {
  public:
    
    enum {
      NoteOn = 0x90,
      NoteOff = 0x80
    };
    
    NoteEvent(unsigned char type, int stp, int val, int vel);

    /** Return the second data byte of this event. For a note event this will
  be the MIDI note number, between 0 and 127. */
    unsigned char get_key() const;
    /** Return the third data byte of this event. For a note event this will be
  the key velocity, between 0 and 127. */
    unsigned char get_velocity() const;
    /** Return the pattern step where this event occurs. */
    unsigned int get_step() const;
    /** Return a pointer to the previous event at the same pattern step. */
    NoteEvent* get_previous() const;
    /** Return a pointer to the next note event at the same pattern step.
  This is overridden from MIDIEvent so we don't have to static_cast
  the returned event pointer to a NoteEvent* every time. */
    NoteEvent* get_next() const;
    /** Return a pointer to the Note object that owns this event. */
    Note* get_note();
    /** Return the MIDI data for this event. */
    const unsigned char* get_data() const;
    
    /** Set the second byte of the event. For note events this is the MIDI
  note number (from 0 to 127). */
    void set_key(unsigned char note);
    /** Set the third byte of the event. For note events this is the key
  velocity (from 0 to 127). */
    void set_velocity(unsigned char vel);
    /** Set the pattern step where this event occurs. */
    void set_step(unsigned int step);
    /** Set the pointer to the previous event at the same pattern step. */
    void set_previous(NoteEvent* event);
    /** Set the pointer to the Note object that owns this event. */
    void set_note(Note* note);
    /** Set the pointer to next event at the same pattern step. */
    void set_next(NoteEvent* next);
    
  protected:
    
    /** The pattern step where this note event occurs. */
    unsigned int m_step;
    /** A pointer to the previous event at the same pattern step. */
    NoteEvent* m_previous;
    /** A pointer to next event at the same pattern step. */
    NoteEvent* m_next;
    /** The Note object that owns this event. */
    Note* m_note;
    
    unsigned char m_data[4];
  };


}


#endif
