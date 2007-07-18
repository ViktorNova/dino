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

#include "argument.hpp"
#include "commandproxy.hpp"
#include "dinoobject.hpp"


DinoObject::DinoObject(Dino::CommandProxy& proxy)
  : m_proxy(proxy) {
  
  add_method("org.nongnu.dino.Sequencer", "Play", "", 
	     sigc::mem_fun(*this, &DinoObject::play));
  add_method("org.nongnu.dino.Sequencer", "Stop", "", 
	     sigc::mem_fun(*this, &DinoObject::stop));
  add_method("org.nongnu.dino.Sequencer", "GoToBeat", "d", 
	     sigc::mem_fun(*this, &DinoObject::go_to_beat));
  add_method("org.nongnu.dino.Song", "SetSongTitle", "s",
	     sigc::mem_fun(*this, &DinoObject::set_song_title));
  add_method("org.nongnu.dino.Song", "SetSongAuthor", "s",
	     sigc::mem_fun(*this, &DinoObject::set_song_author));
  add_method("org.nongnu.dino.Song", "SetSongInfo", "s",
	     sigc::mem_fun(*this, &DinoObject::set_song_info));
  add_method("org.nongnu.dino.Song", "SetSongLength", "i",
	     sigc::mem_fun(*this, &DinoObject::set_song_length));
  add_method("org.nongnu.dino.Song", "SetLoopStart", "i",
	     sigc::mem_fun(*this, &DinoObject::set_loop_start));
  add_method("org.nongnu.dino.Song", "SetLoopEnd", "i",
	     sigc::mem_fun(*this, &DinoObject::set_loop_end));
  add_method("org.nongnu.dino.Song", "AddTrack", "s",
	     sigc::mem_fun(*this, &DinoObject::add_track));
  add_method("org.nongnu.dino.Song", "RemoveTrack", "i",
	     sigc::mem_fun(*this, &DinoObject::remove_track));
  add_method("org.nongnu.dino.Song", "AddTempoChange", "id",
	     sigc::mem_fun(*this, &DinoObject::add_tempo_change));
  add_method("org.nongnu.dino.Song", "RemoveTempoChange", "i",
	     sigc::mem_fun(*this, &DinoObject::remove_tempo_change));
  
}

bool DinoObject::play(int argc, DBus::Argument* argv) {
  m_proxy.play();
  return true;
}


bool DinoObject::stop(int argc, DBus::Argument* argv) {
  m_proxy.stop();
  return true;
}


bool DinoObject::go_to_beat(int argc, DBus::Argument* argv) {
  m_proxy.go_to_beat(argv[0].d);
  return true;
}


bool DinoObject::set_song_title(int argc, DBus::Argument* argv) {
  return m_proxy.set_song_title(argv[0].s);
}


bool DinoObject::set_song_author(int argc, DBus::Argument* argv) {
  return m_proxy.set_song_author(argv[0].s);
}


bool DinoObject::set_song_info(int argc, DBus::Argument* argv) {
  return m_proxy.set_song_info(argv[0].s);
}


bool DinoObject::set_song_length(int argc, DBus::Argument* argv) {
  return m_proxy.set_song_length(argv[0].i);
}


bool DinoObject::set_loop_start(int argc, DBus::Argument* argv) {
  return m_proxy.set_loop_start(argv[0].i);
}


bool DinoObject::set_loop_end(int argc, DBus::Argument* argv) {
  return m_proxy.set_loop_end(argv[0].i);
}


bool DinoObject::add_track(int argc, DBus::Argument* argv) {
  return m_proxy.add_track(argv[0].s);
}


bool DinoObject::remove_track(int argc, DBus::Argument* argv) {
  return m_proxy.remove_track(argv[0].i);
}


bool DinoObject::add_tempo_change(int argc, DBus::Argument* argv) {
  return m_proxy.add_tempo_change(argv[0].i, argv[1].d);
}


bool DinoObject::remove_tempo_change(int argc, DBus::Argument* argv) {
  return m_proxy.remove_tempo_change(argv[0].i);
}
