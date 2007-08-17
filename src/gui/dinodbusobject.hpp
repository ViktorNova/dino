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
}


namespace DBus {
  class Argument;
}


/** A D-Bus object that wraps all sequencer and song operations in Dino,
    so they can be accessed by external programs and scripts. */
class DinoDBusObject : public DBus::Object {
public:
  
  DinoDBusObject(Dino::CommandProxy& proxy);

protected:
  
  bool play(int argc, DBus::Argument* argv);
  bool stop(int argc, DBus::Argument* argv);
  bool go_to_beat(int argc, DBus::Argument* argv);
  
  bool set_song_title(int argc, DBus::Argument* argv);
  bool set_song_author(int argc, DBus::Argument* argv);
  bool set_song_info(int argc, DBus::Argument* argv);
  bool set_song_length(int argc, DBus::Argument* argv);
  bool set_loop_start(int argc, DBus::Argument* argv);
  bool set_loop_end(int argc, DBus::Argument* argv);
  bool add_track(int argc, DBus::Argument* argv);
  bool remove_track(int argc, DBus::Argument* argv);
  bool add_tempo_change(int argc, DBus::Argument* argv);
  bool remove_tempo_change(int argc, DBus::Argument* argv);
  
  bool set_track_name(int argc, DBus::Argument* argv);
  bool add_pattern(int argc, DBus::Argument* argv);
  bool duplicate_pattern(int argc, DBus::Argument* argv);
  bool remove_pattern(int argc, DBus::Argument* argv);
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

  bool set_pattern_name(int argc, DBus::Argument* argv);
  bool set_pattern_length(int argc, DBus::Argument* argv);
  bool set_pattern_steps(int argc, DBus::Argument* argv);
  bool add_note(int argc, DBus::Argument* argv);
  bool set_note_velocity(int argc, DBus::Argument* argv);
  bool set_note_size(int argc, DBus::Argument* argv);
  bool delete_note(int argc, DBus::Argument* argv);

  Dino::CommandProxy& m_proxy;

};


#endif
