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
  
  DinoDBusObject(Dino::CommandProxy& proxy, Dino::Sequencer& sequencer);

protected:
  
  /** Start playing at the current position. Does nothing if the song is
      already playing. */
  bool play(int argc, DBus::Argument* argv);
  
  /** Stop playing and stay at the current position. Does nothing if the song
      isn't currently playing. */
  bool stop(int argc, DBus::Argument* argv);
  
  /** Move the play cursor to the given position in beats. */
  bool go_to_beat(int argc, DBus::Argument* argv);
  
  
  /** Change the title of the song. */
  bool set_song_title(int argc, DBus::Argument* argv);
  
  /** Change the author of the song. */
  bool set_song_author(int argc, DBus::Argument* argv);
  
  /** Change the information text about the song. */
  bool set_song_info(int argc, DBus::Argument* argv);
  
  /** Change the length of the song (deleting any sequence parts that are 
      after the new length). */
  bool set_song_length(int argc, DBus::Argument* argv);
  
  /** Change the loop start point. */
  bool set_loop_start(int argc, DBus::Argument* argv);
  
  /** Change the loop end point. */
  bool set_loop_end(int argc, DBus::Argument* argv);
  
  /** Add a new track. */
  bool add_track(int argc, DBus::Argument* argv);
  
  /** Remove a track and all its patterns and controllers. */
  bool remove_track(int argc, DBus::Argument* argv);

  /** Add a tempo change. */
  bool add_tempo_change(int argc, DBus::Argument* argv);
  
  /** Remove a tempo change. */
  bool remove_tempo_change(int argc, DBus::Argument* argv);
  
  
  bool set_track_name(int argc, DBus::Argument* argv);
  bool add_pattern(int argc, DBus::Argument* argv);
  bool duplicate_pattern(int argc, DBus::Argument* argv);
  bool remove_pattern(int argc, DBus::Argument* argv);
  bool add_sequence_entry(int argc, DBus::Argument* argv);
  bool remove_sequence_entry(int argc, DBus::Argument* argv);
  bool set_sequence_entry_length(int argc, DBus::Argument* argv);
  bool set_track_midi_channel(int argc, DBus::Argument* argv);
  bool add_controller(int argc, DBus::Argument* argv);
  bool remove_controller(int argc, DBus::Argument* argv);
  bool set_controller_name(int argc, DBus::Argument* argv);
  bool set_controller_min(int argc, DBus::Argument* argv);
  bool set_controller_max(int argc, DBus::Argument* argv);
  bool set_controller_default(int argc, DBus::Argument* argv);
  bool set_controller_number(int argc, DBus::Argument* argv);
  bool set_controller_global(int argc, DBus::Argument* argv);
  bool add_key(int argc, DBus::Argument* argv);
  bool remove_key(int argc, DBus::Argument* argv);
  bool set_key_name(int argc, DBus::Argument* argv);
  bool set_key_number(int argc, DBus::Argument* argv);

  bool set_pattern_name(int argc, DBus::Argument* argv);
  bool set_pattern_length(int argc, DBus::Argument* argv);
  bool set_pattern_steps(int argc, DBus::Argument* argv);
  bool add_note(int argc, DBus::Argument* argv);
  bool set_note_velocity(int argc, DBus::Argument* argv);
  bool set_note_size(int argc, DBus::Argument* argv);
  bool delete_note(int argc, DBus::Argument* argv);
  bool add_curve_point(int argc, DBus::Argument* argv);
  bool remove_curve_point(int argc, DBus::Argument* argv);

  
  Dino::CommandProxy& m_proxy;
  Dino::Sequencer& m_seq;
  
};


#endif
