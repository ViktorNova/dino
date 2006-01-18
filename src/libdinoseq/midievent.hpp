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

#include "deletable.hpp"


namespace Dino {


  /** This class represents a single MIDI event. It contains 4 bytes of
      raw MIDI data and a pointer to next event (which is used to pass lists
      of events to the sequencer). This class defines the interface that
      the sequencer will use for all MIDI events, but the Pattern class uses
      other event classes internally (which are subclasses of MIDIEvent). */
  class MIDIEvent : public Deletable {
  public:
  
    MIDIEvent(unsigned char type, unsigned char data1, unsigned char data2) 
      : m_next(NULL) {
      m_data[0] = type;
      m_data[1] = data1;
      m_data[2] = data2;
    }
    
    /** Return the status byte of the event with any eventual channel part
	masked out. If the event is a Note Off 0x80 will be returned, if it
	is a Tune Request 0xF6 will be returned etc. */
    unsigned char get_type() const;
    /** Return the second data byte of this event. For a note event this will
	be the MIDI note number, between 0 and 127. */
    unsigned char get_note() const;
    /** Return the low nibble of the status byte. For channel specific events
	like note events and controllers, this will be the MIDI channel. */
    unsigned char get_channel() const;
    /** Return the third data byte of this event. For a note event this will be
	the key velocity, between 0 and 127. */
    unsigned char get_velocity() const;
    /** Return the third data byte of this event. For a controller event this
	will be the controller value. */
    unsigned char get_value() const;
    /** Return a pointer to next event, which is used to pass linked lists
	of MIDI events to the sequencer. */
    MIDIEvent* get_next() const;
    /** Return a pointer to an array of the 4 MIDI data bytes. */
    const unsigned char* get_data() const;
    /** Return the number of data bytes that are actually used for this 
	event. */
    unsigned int get_size() const;
    
    /** Set the status byte of this event. */
    void set_type(unsigned char type);
    /** Set the low nibble of the status byte. For channel specific events
	this is the MIDI channel. */
    void set_channel(unsigned char channel);
    /** Set the second byte of the event. For note events this is the MIDI
	note number (from 0 to 127). */
    void set_note(unsigned char note);
    /** Set the third byte of the event. For note events this is the key
	velocity (from 0 to 127). */
    void set_velocity(unsigned char vel);
    /** Set the third byte of the event. For controller events this is the
	controller value. */
    void set_value(unsigned char value);
    /** Set the pointer to next MIDI event. This is used to build linked lists
	of events to be passed to the sequencer. */
    void set_next(MIDIEvent* event);
    
    /* Some common types. */
    static const unsigned char NoteOn = 0x90;
    static const unsigned char NoteOff = 0x80;
    static const unsigned char Controller = 0xB0;
    static const unsigned char PitchWheel = 0xE0;
    
    /* These global MIDIEvent objects are used by the sequencer. */
    static MIDIEvent AllNotesOff;
    static MIDIEvent ChannelVolume;
    
  protected:
    
    /** A pointer to next event in the list. */
    MIDIEvent* m_next;
    /** The actual MIDI data. All four bytes aren't always used. */
    unsigned char m_data[4];
    
  };


}


#endif
