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

#ifndef DINODBUSOBJECT_HPP
#define DINODBUSOBJECT_HPP

#include "dbus/object.hpp"


namespace Dino {
  class CommandProxy;
  class Sequencer;
}


namespace DBus {
  class Argument;
}


/** A D-Bus object that wraps all sequencer and song operations in Dino,
    so they can be accessed by external programs and scripts. */
class DinoDBusObject : public DBus::Object {
public:
  
  /** Create a new DinoDBusObject to control the given proxy and sequencer. */
  DinoDBusObject(Dino::CommandProxy& proxy, Dino::Sequencer& sequencer);

protected:
  
  /** Utility function that creates a return list for a method that doesn't
      return anything. If the parameter is @c true it returns 0, if it is 
      @c false it allocates and returns a DBus::Argument array with a single
      element with the argument type DBus::Argument::ERROR (and a trailing
      DBus::Argument::INVALID). */
  static DBus::Argument* create_result(bool result);
  
  /** Start playing at the current position. Does nothing if the song is
      already playing. */
  DBus::Argument* play(int argc, DBus::Argument* argv);
  
  /** Stop playing and stay at the current position. Does nothing if the song
      isn't currently playing. */
  DBus::Argument* stop(int argc, DBus::Argument* argv);
  
  /** Move the play cursor to the given position in beats. */
  DBus::Argument* go_to_beat(int argc, DBus::Argument* argv);
  
  
  /** Change the title of the song. */
  DBus::Argument* set_song_title(int argc, DBus::Argument* argv);
  
  /** Change the author of the song. */
  DBus::Argument* set_song_author(int argc, DBus::Argument* argv);
  
  /** Change the information text about the song. */
  DBus::Argument* set_song_info(int argc, DBus::Argument* argv);
  
  /** Change the length of the song (deleting any sequence parts that are 
      after the new length). */
  DBus::Argument* set_song_length(int argc, DBus::Argument* argv);
  
  /** Change the loop start point. */
  DBus::Argument* set_loop_start(int argc, DBus::Argument* argv);
  
  /** Change the loop end point. */
  DBus::Argument* set_loop_end(int argc, DBus::Argument* argv);
  
  /** Add a new track. */
  DBus::Argument* add_track(int argc, DBus::Argument* argv);
  
  /** Remove a track and all its patterns and controllers. */
  DBus::Argument* remove_track(int argc, DBus::Argument* argv);

  /** Add a tempo change. */
  DBus::Argument* add_tempo_change(int argc, DBus::Argument* argv);
  
  /** Remove a tempo change. */
  DBus::Argument* remove_tempo_change(int argc, DBus::Argument* argv);
  
  
  /** Set the name of the track. */
  DBus::Argument* set_track_name(int argc, DBus::Argument* argv);
  
  /** Add a new pattern to the track. */
  DBus::Argument* add_pattern(int argc, DBus::Argument* argv);
  
  /** Create a copy of an existing pattern. */
  DBus::Argument* duplicate_pattern(int argc, DBus::Argument* argv);
  
  /** Remove a pattern from the track and delete it. */
  DBus::Argument* remove_pattern(int argc, DBus::Argument* argv);
  
  /** Add a new sequence entry in the track. */
  DBus::Argument* add_sequence_entry(int argc, DBus::Argument* argv);
  
  /** Remove a sequence entry from the track. */
  DBus::Argument* remove_sequence_entry(int argc, DBus::Argument* argv);
  
  /** Change the length of a sequence entry. */
  DBus::Argument* set_sequence_entry_length(int argc, DBus::Argument* argv);
  
  /** Change the MIDI channel that this track sends MIDI events to. */
  DBus::Argument* set_track_midi_channel(int argc, DBus::Argument* argv);
  
  /** Add a controller to this track. */
  DBus::Argument* add_controller(int argc, DBus::Argument* argv);
  
  /** Remove a controller from this track. */
  DBus::Argument* remove_controller(int argc, DBus::Argument* argv);
  
  /** Set the name of a controller. */
  DBus::Argument* set_controller_name(int argc, DBus::Argument* argv);
  
  /** Set the minimum value of a controller. */
  DBus::Argument* set_controller_min(int argc, DBus::Argument* argv);
  
  /** Set the maximum value of a controller. */
  DBus::Argument* set_controller_max(int argc, DBus::Argument* argv);
  
  /** Set the default value of a controller. */
  DBus::Argument* set_controller_default(int argc, DBus::Argument* argv);
  
  /** Set the ID number of a controller. */
  DBus::Argument* set_controller_number(int argc, DBus::Argument* argv);
  
  /** Set whether a controller has a single curve for the whole track
      or one curve per pattern. */
  DBus::Argument* set_controller_global(int argc, DBus::Argument* argv);
  
  /** Add a named key. */
  DBus::Argument* add_key(int argc, DBus::Argument* argv);
  
  /** Remove a named key. */
  DBus::Argument* remove_key(int argc, DBus::Argument* argv);
  
  /** Set the name of a named key. */
  DBus::Argument* set_key_name(int argc, DBus::Argument* argv);
  
  /** Set the MIDI key number of a named key. */
  DBus::Argument* set_key_number(int argc, DBus::Argument* argv);
  
  
  /** Change the name of a pattern. */
  DBus::Argument* set_pattern_name(int argc, DBus::Argument* argv);
  
  /** Change the length of a pattern, deleting all notes and controller values
      after the new length. */
  DBus::Argument* set_pattern_length(int argc, DBus::Argument* argv);
  
  /** Change the number of steps per beat in the pattern. */
  DBus::Argument* set_pattern_steps(int argc, DBus::Argument* argv);
  
  /** Add a note to the pattern. */
  DBus::Argument* add_note(int argc, DBus::Argument* argv);
  
  /** Set the velocity of a note. */
  DBus::Argument* set_note_velocity(int argc, DBus::Argument* argv);
  
  /** Set the length of a note. */
  DBus::Argument* set_note_size(int argc, DBus::Argument* argv);
  
  /** Delete a note from a pattern. */
  DBus::Argument* delete_note(int argc, DBus::Argument* argv);
  
  /** Add a curve point. */
  DBus::Argument* add_curve_point(int argc, DBus::Argument* argv);
  
  /** Remove a curve points. */
  DBus::Argument* remove_curve_point(int argc, DBus::Argument* argv);

  
  /** The global command proxy object. */
  Dino::CommandProxy& m_proxy;
  
  /** The sequencer. */
  Dino::Sequencer& m_seq;
  
};


#endif
