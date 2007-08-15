/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006-2007  Lars Luthman <lars.luthman@gmail.com>
   
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

#include <iostream>

#include "commandproxy.hpp"
#include "genericcommands.hpp"
#include "sequencer.hpp"
#include "song.hpp"
#include "songcommands.hpp"


namespace Dino {


  CommandProxy::CommandProxy(Song& song, Sequencer& seq)
    : m_song(song),
      m_seq(seq),
      m_active(false) {

  }
    
  
  bool CommandProxy::can_undo() const {
    return !m_stack.empty();
  }
  
  
  std::string CommandProxy::get_next_undo_name() const {
    if (m_stack.empty())
      return "";
    return m_stack.top()->get_name();
  }
  

  bool CommandProxy::undo() {
    if (m_active)
      return false;
    m_active = true;
    std::cerr<<__PRETTY_FUNCTION__<<std::endl;
    if (m_stack.empty())
      return false;
    Command* cmd = m_stack.top();
    m_stack.pop();
    cmd->undo_command();
    delete cmd;
    if (m_stack.empty())
      std::cerr<<"Stack is empty!"<<std::endl;
    else
      std::cerr<<"Stack is not empty!"<<std::endl;
    m_signal_stack_changed();
    m_active = false;
    return true;
  }
    
  
  void CommandProxy::play() {
    m_seq.play();
  }
  
  
  void CommandProxy::stop() {
    m_seq.stop();
  }
  
  
  void CommandProxy::go_to_beat(double beat) {
    m_seq.go_to_beat(beat);
  }
  
  
  bool CommandProxy::set_song_title(const std::string& title) {
    return push_and_do(new SetString<Song>("Change song title", 
					   m_song, title,
					   &Song::get_title, 
					   &Song::set_title));
  }
  
  
  bool CommandProxy::set_song_author(const std::string& author) {
    return push_and_do(new SetString<Song>("Change song author", 
					   m_song, author,
					   &Song::get_author, 
					   &Song::set_author));
  }
  
  
  bool CommandProxy::set_song_info(const std::string& info) {
    return push_and_do(new SetString<Song>("Change song info", 
					   m_song, info,
					   &Song::get_info, 
					   &Song::set_info));
  }


  bool CommandProxy::set_song_length(int length) {
    return push_and_do(new SetSongLength(m_song, length));
  }


  bool CommandProxy::set_loop_start(int beat) {
    return push_and_do(new SetLoopStart(m_song, beat));
  }


  bool CommandProxy::set_loop_end(int beat) {
    return push_and_do(new SetLoopEnd(m_song, beat));
  }
  

  bool CommandProxy::add_track(const std::string& name, 
			       Song::TrackIterator* iter) {
    return push_and_do(new AddTrack(m_song, name, iter));
  }


  bool CommandProxy::remove_track(int id) {
    return push_and_do(new RemoveTrack(m_song, id));
  }

  
  bool CommandProxy::add_tempo_change(int beat, double bpm, 
				      Song::TempoIterator* iter) {
    return push_and_do(new AddTempoChange(m_song, beat, bpm, iter));
  }

  
  bool CommandProxy::remove_tempo_change(unsigned long beat) {
    return push_and_do(new RemoveTempoChange(m_song, beat));
  }
  
  
  bool CommandProxy::set_track_name(int id, const std::string& name) {
    return push_and_do(new SetTrackName(m_song, id, name));
  }

 
  bool CommandProxy::add_pattern(int track, const std::string& name, 
				 int length, int steps, 
				 Track::PatternIterator* iter) {
    return push_and_do(new AddPattern(m_song, track, name, 
				      length, steps, iter));
  }
  
  
  bool CommandProxy::duplicate_pattern(int track, int pattern, 
				       Track::PatternIterator* iter) {
    return push_and_do(new DuplicatePattern(m_song, track, pattern, iter));
  }


  bool CommandProxy::remove_pattern(int track, int pattern) {
    return push_and_do(new RemovePattern(m_song, track, pattern));
  }
  

  bool CommandProxy::remove_sequence_entry(int track, unsigned long beat) {
    return push_and_do(new RemoveSequenceEntry(m_song, track, beat));
  }
 

  bool CommandProxy::set_sequence_entry_length(int track, unsigned long beat, 
					       unsigned int length) {
    return push_and_do(new SetSequenceEntryLength(m_song, track, beat, length));
  }
  
  
  bool CommandProxy::set_track_midi_channel(int track, int channel) {
    return push_and_do(new SetTrackMidiChannel(m_song, track, channel));
  }


  bool CommandProxy::add_controller(int track, long number, 
				    const std::string& name,
				    int default_v, int min, int max, 
				    bool global) {
    return push_and_do(new AddController(m_song, track, number, name, 
					 default_v, min, max, global));
  }


  bool CommandProxy::remove_controller(int track, long number) {
    return push_and_do(new RemoveController(m_song, track, number));
  }


  bool CommandProxy::set_controller_name(int track, long number, 
					 const std::string& name) {
    return push_and_do(new SetControllerName(m_song, track, number, name));
  }


  bool CommandProxy::set_controller_min(int track, long number, int min) {
    return push_and_do(new SetControllerMin(m_song, track, number, min));
  }


  bool CommandProxy::set_controller_max(int track, long number, int max) {
    return push_and_do(new SetControllerMax(m_song, track, number, max));
  }


  bool CommandProxy::set_controller_default(int track, long number, 
					    int _default) {
    return push_and_do(new SetControllerDefault(m_song, track, number, 
						_default));
  }


  bool CommandProxy::set_controller_number(int track, long old_number, 
					   long new_number) {
    return push_and_do(new SetControllerNumber(m_song, track, old_number, 
					       new_number));
  }


  bool CommandProxy::set_controller_global(int track, long number, 
					   bool global) {
    return push_and_do(new SetControllerGlobal(m_song, track, number, global));
  }


  sigc::signal<void>& CommandProxy::signal_stack_changed() {
    return m_signal_stack_changed;
  }
  

  bool CommandProxy::push_and_do(Command* cmd) {
    if (m_active) {
      delete cmd;
      return false;
    }
    m_active = true;
    if (cmd->do_command()) {
      m_stack.push(cmd);
      m_signal_stack_changed();
      m_active = false;
      return true;
    }
    m_active = false;
    return false;
  }
  
  
}
