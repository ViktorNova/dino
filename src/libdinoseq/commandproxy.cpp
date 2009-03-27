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
#include "compoundcommand.hpp"
#include "debug.hpp"
#include "genericcommands.hpp"
#include "song.hpp"
#include "songcommands.hpp"


namespace Dino {


  CommandProxy::CommandProxy(Song& song)
    : m_song(song),
      m_active(false),
      m_atomic(false),
      m_atomic_count(0) {

  }
    
  
  bool CommandProxy::can_undo() const {
    return !m_stack.empty();
  }
  
  
  std::string CommandProxy::get_next_undo_name() const {
    if (m_stack.empty()) {
      dbg(1, "The stack is now empty");
      return "";
    }
    dbg(1, cc + "The stack top is now " + m_stack.front());
    dbg(1, cc + "The stack size is now " + m_stack.size());
    return m_stack.front()->get_name();
  }
  

  const std::deque<Command*>& CommandProxy::get_undo_stack() const {
    return m_stack;
  }
  

  bool CommandProxy::undo() {
    if (m_active)
      return false;
    
    if (m_atomic_count > 0) {
      dbg(1, "undo() called while an atomic command was being queued!");
      delete m_atomic;
      m_atomic = 0;
      m_atomic_count = 0;
      return true;
    }
    
    m_active = true;
    std::cerr<<__PRETTY_FUNCTION__<<std::endl;
    if (m_stack.empty())
      return false;
    Command* cmd = m_stack.front();
    m_stack.pop_front();
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


  bool CommandProxy::start_atomic(const std::string& name) {
    if (m_atomic_count == 0)
      m_atomic = new CompoundCommand(name);
    ++m_atomic_count;
    return true;
  }
    
  
  bool CommandProxy::end_atomic() {
    if (m_atomic_count == 0)
      return false;
    --m_atomic_count;
    if (m_atomic_count == 0) {
      Command* cmd = m_atomic;
      m_atomic = 0;
      return push_and_do(cmd);
    }
    return true;
  }
    
  
  const Song& CommandProxy::get_song() const {
    return m_song;
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


  bool CommandProxy::set_loop_start(const SongTime& time) {
    return push_and_do(new SetLoopStart(m_song, time));
  }


  bool CommandProxy::set_loop_end(const SongTime& time) {
    return push_and_do(new SetLoopEnd(m_song, time));
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
				 const SongTime& length, int steps, 
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
  

  bool CommandProxy::add_sequence_entry(int track, const SongTime& beat, 
					int pattern, const SongTime& length) {
    return push_and_do(new AddSequenceEntry(m_song, track, 
					    beat, pattern, length));
  }
  
  
  bool CommandProxy::remove_sequence_entry(int track, const SongTime& beat) {
    return push_and_do(new RemoveSequenceEntry(m_song, track, beat));
  }
 

  bool CommandProxy::set_sequence_entry_length(int track, const SongTime& beat,
					       const SongTime& length) {
    return push_and_do(new SetSequenceEntryLength(m_song, track, beat, length));
  }
  
  
  bool CommandProxy::set_track_midi_channel(int track, int channel) {
    return push_and_do(new SetTrackMidiChannel(m_song, track, channel));
  }


  bool CommandProxy::add_controller(int track, uint32_t number, 
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
  
  
  bool CommandProxy::add_key(int track, unsigned char number, 
			     const std::string& name) {
    return push_and_do(new AddKey(m_song, track, number, name));
  }
  
  
  bool CommandProxy::remove_key(int track, unsigned char number) {
    return push_and_do(new RemoveKey(m_song, track, number));
  }

  
  bool CommandProxy::set_key_name(int track, unsigned char number, 
				  const std::string& name) {
    return push_and_do(new SetKeyName(m_song, track, number, name));
  }
  
  
  bool CommandProxy::set_key_number(int track, unsigned char old_number, 
				    unsigned char new_number){
    return push_and_do(new SetKeyNumber(m_song, track, old_number, new_number));
  }

  
  bool CommandProxy::set_track_mode(int track, Track::Mode mode) {
    return push_and_do(new SetTrackMode(m_song, track, mode));
  }

  
  bool CommandProxy::set_pattern_name(int track, int pattern, 
				      const std::string& name) {
    return push_and_do(new SetPatternName(m_song, track, pattern, name));
  }


  bool CommandProxy::set_pattern_length(int track, int pattern, 
					const SongTime& beats) {
    return push_and_do(new SetPatternLength(m_song, track, pattern, beats));
  }


  bool CommandProxy::set_pattern_steps(int track, int pattern, 
				       unsigned int steps) {
    return push_and_do(new SetPatternSteps(m_song, track, pattern, steps));
  }


  bool CommandProxy::add_note(int track, int pattern, const SongTime& step, 
			      int key, int velocity, const SongTime& length) {
    return push_and_do(new AddNote(m_song, track, pattern, 
				   step, key, velocity, length));
  }
  

  bool CommandProxy::add_notes(int track, int pattern, 
			       const NoteCollection& notes, 
			       const SongTime& start, 
			       int key, NoteSelection* selection) {
    return push_and_do(new AddNotes(m_song, track, pattern, notes, 
				    start, key, selection));
  }


  bool CommandProxy::set_note_velocity(int track, int pattern, int step, 
				       int key, int velocity) {
    return push_and_do(new SetNoteVelocity(m_song, track, pattern, step,
					   key, velocity));
  }


  bool CommandProxy::set_note_size(int track, int pattern, 
				   const SongTime& step, int key, 
				   const SongTime& size) {
    return push_and_do(new SetNoteSize(m_song, track, pattern, step, key, 
				       size));
  }

  
  bool CommandProxy::delete_note(int track, int pattern, 
				 const SongTime& step, int key) {
    return push_and_do(new DeleteNote(m_song, track, pattern, step, key));
  }


  bool CommandProxy::add_curve_point(int track, int pattern, 
				     uint32_t number, const SongTime& step, 
				     int value) {
    if (pattern != -1) {
      return push_and_do(new AddPatternCurvePoint(m_song, track, pattern, 
						  number, step, value));
    }
    return push_and_do(new AddTrackCurvePoint(m_song, track, 
					      number, step, value));
  }
  
  
  bool CommandProxy::remove_curve_point(int track, int pattern, 
					uint32_t number, 
					const SongTime& step) {
    push_and_do(new RemovePatternCurvePoint(m_song, track, pattern,
					    number, step));
  }
  

  sigc::signal<void>& CommandProxy::signal_stack_changed() {
    return m_signal_stack_changed;
  }
  

  bool CommandProxy::push_and_do(Command* cmd) {
    
    if (m_active) {
      dbg(0, "A command is already running!");
      delete cmd;
      return false;
    }
    
    if (m_atomic) {
      m_atomic->append(cmd);
      return true;
    }
    
    m_active = true;
    if (cmd->do_command()) {
      m_stack.push_front(cmd);
      m_signal_stack_changed();
      m_active = false;
      return true;
    }
    else {
      dbg(0, cc+ "Command '" + cmd->get_name() + "' failed!");
      delete cmd;
    }
    m_active = false;
    return false;
  }
  
  
}
