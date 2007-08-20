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

#include <sigc++/sigc++.h>

#include "dbus/argument.hpp"

#include "commandproxy.hpp"
#include "dinodbusobject.hpp"
#include "sequencer.hpp"


DinoDBusObject::DinoDBusObject(Dino::CommandProxy& proxy, 
			       Dino::Sequencer& sequencer)
  : m_proxy(proxy),
    m_seq(sequencer) {
  
  add_method("org.nongnu.dino.Sequencer", "Play", "", 
	     sigc::mem_fun(*this, &DinoDBusObject::play));
  add_method("org.nongnu.dino.Sequencer", "Stop", "", 
	     sigc::mem_fun(*this, &DinoDBusObject::stop));
  add_method("org.nongnu.dino.Sequencer", "GoToBeat", "d", 
	     sigc::mem_fun(*this, &DinoDBusObject::go_to_beat));

  add_method("org.nongnu.dino.Song", "SetTitle", "s",
	     sigc::mem_fun(*this, &DinoDBusObject::set_song_title));
  add_method("org.nongnu.dino.Song", "SetAuthor", "s",
	     sigc::mem_fun(*this, &DinoDBusObject::set_song_author));
  add_method("org.nongnu.dino.Song", "SetInfo", "s",
	     sigc::mem_fun(*this, &DinoDBusObject::set_song_info));
  add_method("org.nongnu.dino.Song", "SetLength", "i",
	     sigc::mem_fun(*this, &DinoDBusObject::set_song_length));
  add_method("org.nongnu.dino.Song", "SetLoopStart", "i",
	     sigc::mem_fun(*this, &DinoDBusObject::set_loop_start));
  add_method("org.nongnu.dino.Song", "SetLoopEnd", "i",
	     sigc::mem_fun(*this, &DinoDBusObject::set_loop_end));
  add_method("org.nongnu.dino.Song", "AddTrack", "s",
	     sigc::mem_fun(*this, &DinoDBusObject::add_track));
  add_method("org.nongnu.dino.Song", "RemoveTrack", "i",
	     sigc::mem_fun(*this, &DinoDBusObject::remove_track));
  add_method("org.nongnu.dino.Song", "AddTempoChange", "id",
	     sigc::mem_fun(*this, &DinoDBusObject::add_tempo_change));
  add_method("org.nongnu.dino.Song", "RemoveTempoChange", "i",
	     sigc::mem_fun(*this, &DinoDBusObject::remove_tempo_change));

  add_method("org.nongnu.dino.Song", "SetTrackName", "is",
	     sigc::mem_fun(*this, &DinoDBusObject::set_track_name));
  add_method("org.nongnu.dino.Song", "AddPattern", "isii",
	     sigc::mem_fun(*this, &DinoDBusObject::add_pattern));
  add_method("org.nongnu.dino.Song", "DuplicatePattern", "ii",
	     sigc::mem_fun(*this, &DinoDBusObject::duplicate_pattern));
  add_method("org.nongnu.dino.Song", "RemovePattern", "ii",
	     sigc::mem_fun(*this, &DinoDBusObject::remove_pattern));
  add_method("org.nongnu.dino.Song", "RemoveSequenceEntry", "ii",
	     sigc::mem_fun(*this, &DinoDBusObject::remove_sequence_entry));
  add_method("org.nongnu.dino.Song", "SetSequenceEntryLength", "iii",
	     sigc::mem_fun(*this, &DinoDBusObject::set_sequence_entry_length));
  add_method("org.nongnu.dino.Song", "SetTrackMidiChannel", "ii",
	     sigc::mem_fun(*this, &DinoDBusObject::set_track_midi_channel));
  add_method("org.nongnu.dino.Song", "AddController", "iisiiii",
	     sigc::mem_fun(*this, &DinoDBusObject::add_controller));
  add_method("org.nongnu.dino.Song", "RemoveController", "ii",
	     sigc::mem_fun(*this, &DinoDBusObject::remove_controller));
  add_method("org.nongnu.dino.Song", "SetControllerName", "iis",
	     sigc::mem_fun(*this, &DinoDBusObject::set_controller_name));
  add_method("org.nongnu.dino.Song", "SetControllerMin", "iii",
	     sigc::mem_fun(*this, &DinoDBusObject::set_controller_min));
  add_method("org.nongnu.dino.Song", "SetControllerMax", "iii",
	     sigc::mem_fun(*this, &DinoDBusObject::set_controller_max));
  add_method("org.nongnu.dino.Song", "SetControllerDefault", "iii",
	     sigc::mem_fun(*this, &DinoDBusObject::set_controller_default));
  add_method("org.nongnu.dino.Song", "SetControllerNumber", "iii",
	     sigc::mem_fun(*this, &DinoDBusObject::set_controller_number));
  add_method("org.nongnu.dino.Song", "SetControllerGlobal", "iii",
	     sigc::mem_fun(*this, &DinoDBusObject::set_controller_global));
  add_method("org.nongnu.dino.Song", "SetPatternName", "iis",
	     sigc::mem_fun(*this, &DinoDBusObject::set_pattern_name));
  add_method("org.nongnu.dino.Song", "SetSatternLength", "iii",
	     sigc::mem_fun(*this, &DinoDBusObject::set_pattern_length));
  add_method("org.nongnu.dino.Song", "SetPatternSteps", "iii",
	     sigc::mem_fun(*this, &DinoDBusObject::set_pattern_steps));
  add_method("org.nongnu.dino.Song", "AddNote", "iiiiii",
	     sigc::mem_fun(*this, &DinoDBusObject::add_note));
  add_method("org.nongnu.dino.Song", "SetNoteVelocity", "iiiii",
	     sigc::mem_fun(*this, &DinoDBusObject::set_note_velocity));
  add_method("org.nongnu.dino.Song", "SetNoteSize", "iiiii",
	     sigc::mem_fun(*this, &DinoDBusObject::set_note_size));
  add_method("org.nongnu.dino.Song", "DeleteNote", "iiii",
	     sigc::mem_fun(*this, &DinoDBusObject::delete_note));
  add_method("org.nongnu.dino.Song", "AddCurvePoint", "iiiii",
	     sigc::mem_fun(*this, &DinoDBusObject::add_curve_point));
  add_method("org.nongnu.dino.Song", "RemoveCurvePoint", "iiii",
	     sigc::mem_fun(*this, &DinoDBusObject::remove_curve_point));


}


bool DinoDBusObject::play(int argc, DBus::Argument* argv) {
  m_seq.play();
  return true;
}


bool DinoDBusObject::stop(int argc, DBus::Argument* argv) {
  m_seq.stop();
  return true;
}


bool DinoDBusObject::go_to_beat(int argc, DBus::Argument* argv) {
  m_seq.go_to_beat(argv[0].d);
  return true;
}


bool DinoDBusObject::set_song_title(int argc, DBus::Argument* argv) {
  return m_proxy.set_song_title(argv[0].s);
}


bool DinoDBusObject::set_song_author(int argc, DBus::Argument* argv) {
  return m_proxy.set_song_author(argv[0].s);
}


bool DinoDBusObject::set_song_info(int argc, DBus::Argument* argv) {
  return m_proxy.set_song_info(argv[0].s);
}


bool DinoDBusObject::set_song_length(int argc, DBus::Argument* argv) {
  return m_proxy.set_song_length(argv[0].i);
}


bool DinoDBusObject::set_loop_start(int argc, DBus::Argument* argv) {
  return m_proxy.set_loop_start(argv[0].i);
}


bool DinoDBusObject::set_loop_end(int argc, DBus::Argument* argv) {
  return m_proxy.set_loop_end(argv[0].i);
}


bool DinoDBusObject::add_track(int argc, DBus::Argument* argv) {
  return m_proxy.add_track(argv[0].s);
}


bool DinoDBusObject::remove_track(int argc, DBus::Argument* argv) {
  return m_proxy.remove_track(argv[0].i);
}


bool DinoDBusObject::add_tempo_change(int argc, DBus::Argument* argv) {
  return m_proxy.add_tempo_change(argv[0].i, argv[1].d);
}


bool DinoDBusObject::remove_tempo_change(int argc, DBus::Argument* argv) {
  return m_proxy.remove_tempo_change(argv[0].i);
}


bool DinoDBusObject::set_track_name(int argc, DBus::Argument* argv) {
  return m_proxy.set_track_name(argv[0].i, argv[1].s);
}


bool DinoDBusObject::add_pattern(int argc, DBus::Argument* argv) {
  return m_proxy.add_pattern(argv[0].i, argv[1].s, argv[2].i, argv[3].i);
}


bool DinoDBusObject::duplicate_pattern(int argc, DBus::Argument* argv) {
  return m_proxy.duplicate_pattern(argv[0].i, argv[1].i);
}


bool DinoDBusObject::remove_pattern(int argc, DBus::Argument* argv) {
  return m_proxy.remove_pattern(argv[0].i, argv[1].i);
}


bool DinoDBusObject::remove_sequence_entry(int argc, DBus::Argument* argv) {
  return m_proxy.remove_sequence_entry(argv[0].i, argv[1].i);
}


bool DinoDBusObject::set_sequence_entry_length(int argc, DBus::Argument* argv) {
  return m_proxy.set_sequence_entry_length(argv[0].i, argv[1].i, argv[2].i);
}


bool DinoDBusObject::set_track_midi_channel(int argc, DBus::Argument* argv) {
  return m_proxy.set_track_midi_channel(argv[0].i, argv[1].i);
}


bool DinoDBusObject::add_controller(int argc, DBus::Argument* argv) {
  return m_proxy.add_controller(argv[0].i, argv[1].i, argv[2].s, argv[3].i,
				argv[4].i, argv[5].i, argv[6].i);
}


bool DinoDBusObject::remove_controller(int argc, DBus::Argument* argv) {
  return m_proxy.remove_controller(argv[0].i, argv[1].i);
}


bool DinoDBusObject::set_controller_name(int argc, DBus::Argument* argv) {
  return m_proxy.set_controller_name(argv[0].i, argv[1].i, argv[2].s);
}


bool DinoDBusObject::set_controller_min(int argc, DBus::Argument* argv) {
  return m_proxy.set_controller_min(argv[0].i, argv[1].i, argv[2].i);
}


bool DinoDBusObject::set_controller_max(int argc, DBus::Argument* argv) {
  return m_proxy.set_controller_max(argv[0].i, argv[1].i, argv[2].i);
}


bool DinoDBusObject::set_controller_default(int argc, DBus::Argument* argv) {
  return m_proxy.set_controller_default(argv[0].i, argv[1].i, argv[2].i);
}


bool DinoDBusObject::set_controller_number(int argc, DBus::Argument* argv) {
  return m_proxy.set_controller_number(argv[0].i, argv[1].i, argv[2].i);
}


bool DinoDBusObject::set_controller_global(int argc, DBus::Argument* argv) {
  return m_proxy.set_controller_global(argv[0].i, argv[1].i, argv[2].i);
}


bool DinoDBusObject::set_pattern_name(int argc, DBus::Argument* argv) {
  return m_proxy.set_pattern_name(argv[0].i, argv[1].i, argv[2].s);
}


bool DinoDBusObject::set_pattern_length(int argc, DBus::Argument* argv) {
  return m_proxy.set_pattern_length(argv[0].i, argv[1].i, argv[2].i);
}


bool DinoDBusObject::set_pattern_steps(int argc, DBus::Argument* argv) {
  return m_proxy.set_pattern_steps(argv[0].i, argv[1].i, argv[2].i);
}


bool DinoDBusObject::add_note(int argc, DBus::Argument* argv) {
  return m_proxy.add_note(argv[0].i, argv[1].i, argv[2].i, 
			  argv[3].i, argv[4].i, argv[5].i);
}


bool DinoDBusObject::set_note_velocity(int argc, DBus::Argument* argv) {
  return m_proxy.set_note_velocity(argv[0].i, argv[1].i, argv[2].i, 
				   argv[3].i, argv[4].i);
}


bool DinoDBusObject::set_note_size(int argc, DBus::Argument* argv) {
  return m_proxy.set_note_velocity(argv[0].i, argv[1].i, argv[2].i, 
				   argv[3].i, argv[4].i);
}


bool DinoDBusObject::delete_note(int argc, DBus::Argument* argv) {
  return m_proxy.delete_note(argv[0].i, argv[1].i, argv[2].i, argv[3].i);
}


bool DinoDBusObject::add_curve_point(int argc, DBus::Argument* argv) {
  return m_proxy.add_curve_point(argv[0].i, argv[1].i, argv[2].i,
				 argv[3].i, argv[4].i);
}


bool DinoDBusObject::remove_curve_point(int argc, DBus::Argument* argv){
  return m_proxy.remove_curve_point(argv[0].i, argv[1].i, argv[2].i, argv[3].i);
}



