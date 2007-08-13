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

#include "controller_numbers.hpp"
#include "deleter.hpp"
#include "pattern.hpp"
#include "song.hpp"
#include "track.hpp"
#include "songcommands.hpp"


// The classes in this namespace are only used internally
namespace {
  
  
  class _SetSongLength : public Dino::Command {
  public:
    
    _SetSongLength(Dino::Song& song, int length)
      : Dino::Command("Change song length [INTERNAL]"),
	m_song(song),
	m_length(length),
	m_oldlength(-1) {

    }
    
    bool do_command() {
      if (m_length < 1)
	return false;
      m_oldlength = m_song.get_length();
      m_song.set_length(m_length);
      return true;
    }
    
    bool undo_command() {
      m_song.set_length(m_oldlength);
      return true;
    }
    
  protected:
    
    Dino::Song& m_song;
    int m_length;
    int m_oldlength;
    
  };


  class _RemovePattern : public Dino::Command {
  public:

    _RemovePattern(Dino::Song& song, int track, int pattern)
      : Dino::Command("Remove pattern"),
	m_song(song),
	m_track(track),
	m_pattern(pattern),
	m_patptr(0) {
      
    }
    
    
    ~_RemovePattern() {
      if (m_patptr)
	Dino::Deleter::queue(m_patptr);
    }
    
    
    bool do_command() {
      Dino::Song::TrackIterator titer = m_song.tracks_find(m_track);
      if (titer == m_song.tracks_end())
	return false;
      Dino::Track::PatternIterator piter = titer->pat_find(m_pattern);
      if (piter == titer->pat_end())
	return false;
      m_patptr = titer->disown_pattern(piter->get_id());
      return (m_patptr != 0);
    }
    
    
    bool undo_command() {
      if (!m_patptr)
	return false;
      Dino::Song::TrackIterator titer = m_song.tracks_find(m_track);
      if (titer == m_song.tracks_end())
	return false;
      Dino::Track::PatternIterator piter = titer->add_pattern(m_patptr);
      if (piter == titer->pat_end())
	return false;
      m_patptr = 0;
      return true;
    }

  protected:

    Dino::Song& m_song;
    int m_track;
    int m_pattern;
    Dino::Pattern* m_patptr;

  };


}


namespace Dino {
  
  
  SetSongLength::SetSongLength(Song& song, int length)
    : CompoundCommand("Change song length"),
      m_song(song),
      m_length(length) {
    
  }
  
  
  bool SetSongLength::do_command() {
    
    clear();
    
    if (m_length < m_song.get_length()) {
      
      // remove or change the loop
      if (m_song.get_loop_start() > m_length)
	append(new SetLoopStart(m_song, -1));
      if (m_song.get_loop_end() > m_length)
	append(new SetLoopEnd(m_song, m_length));
      
      // remove all tempochanges after the new length
      Song::TempoIterator tmpiter = m_song.tempo_find(m_length);
      for ( ; tmpiter != m_song.tempo_end(); ++tmpiter) {
	if (tmpiter->get_beat() >= m_length)
	  append(new RemoveTempoChange(m_song, tmpiter->get_beat()));
      }
      
      // remove or resize all sequence entries that extend beyond the
      // new length
      Song::TrackIterator titer = m_song.tracks_begin();
      for ( ; titer != m_song.tracks_end(); ++titer) {
	Track::SequenceIterator siter = titer->seq_begin();
	for ( ; siter != titer->seq_end(); ++siter) {
	  if (siter->get_start() >= m_length)
	    append(new RemoveSequenceEntry(m_song, titer->get_id(), 
					   siter->get_start()));
	  else if (siter->get_start() + siter->get_length() > m_length)
	    append(new SetSequenceEntryLength(m_song, titer->get_id(),
					      siter->get_start(),
					      m_length - siter->get_start()));
	}
      }

    }
    
    append(new _SetSongLength(m_song, m_length));
    
    CompoundCommand::do_command();
  }
  

  SetLoopStart::SetLoopStart(Song& song, int beat) 
    : Command("Set loop start"),
      m_song(song),
      m_beat(beat),
      m_oldbeat(-1) {

  }
  
  
  bool SetLoopStart::do_command() {
    m_oldbeat = m_song.get_loop_start();
    if (m_beat == m_oldbeat || m_beat > m_song.get_length())
      return false;
    m_song.set_loop_start(m_beat);
    return true;
  }
  
  
  bool SetLoopStart::undo_command() {
    if (m_oldbeat > m_song.get_length())
      return false;
    m_song.set_loop_start(m_oldbeat);
    return true;
  }

  
  SetLoopEnd::SetLoopEnd(Song& song, int beat) 
    : Command("Set loop start"),
      m_song(song),
      m_beat(beat),
      m_oldbeat(-1) {

  }
  
  
  bool SetLoopEnd::do_command() {
    m_oldbeat = m_song.get_loop_end();
    if (m_beat == m_oldbeat || m_beat > m_song.get_length())
      return false;
    m_song.set_loop_end(m_beat);
    return true;
  }
  
  
  bool SetLoopEnd::undo_command() {
    if (m_oldbeat > m_song.get_length())
      return false;
    m_song.set_loop_end(m_oldbeat);
    return true;
  }

  
  AddTrack::AddTrack(Song& song, const std::string& name, 
		     Song::TrackIterator* iter)
    : Command("Add track"),
      m_song(song),
      m_name(name),
      m_id(-1),
      m_iter_store(iter) {

  }
  
  
  bool AddTrack::do_command() {
    Song::TrackIterator titer = m_song.add_track(m_name);
    if (m_iter_store) {
      *m_iter_store = titer;
      m_iter_store = 0;
    }
    if (titer == m_song.tracks_end())
      return false;
    m_id = titer->get_id();
    return true;
  }
  
  
  bool AddTrack::undo_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_id);
    if (titer == m_song.tracks_end())
      return false;
    return m_song.remove_track(titer);
  }
  

  RemoveTrack::RemoveTrack(Song& song, int id)
    : Command("Remove track"),
      m_song(song),
      m_id(id),
      m_trk(0) {

  }
  
  
  RemoveTrack::~RemoveTrack() {
    if (m_trk)
      Deleter::queue(m_trk);
  }
  
  
  bool RemoveTrack::do_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_id);
    if (titer == m_song.tracks_end())
      return false;
    m_trk = m_song.disown_track(titer);
    if (!m_trk)
      return false;
    return true;
  }


  bool RemoveTrack::undo_command() {
    Song::TrackIterator titer = m_song.add_track(m_trk);
    m_trk = 0;
    return (titer != m_song.tracks_end());
  }
  
  
  AddTempoChange::AddTempoChange(Song& song, int beat, double bpm,
				 Song::TempoIterator* iter)
    : Command("Set tempo change"),
      m_song(song),
      m_beat(beat),
      m_bpm(bpm),
      m_oldbpm(-1),
      m_iter_store(iter) {

  }
  
  
  bool AddTempoChange::do_command() {
    if (m_beat < 0 || m_beat >= m_song.get_length()) {
      if (m_iter_store) {
	*m_iter_store = m_song.tempo_end();
	m_iter_store = 0;
      }
      return false;
    }
    m_oldbpm = -1;
    Song::TempoIterator iter = m_song.tempo_find(m_beat);
    if (iter->get_beat() == m_beat) {
      if (iter->get_bpm() == m_bpm) {
	if (m_iter_store) {
	  *m_iter_store = m_song.tempo_end();
	  m_iter_store = 0;
	}
	return false;
      }
      m_oldbpm = iter->get_bpm();
    }
    iter = m_song.add_tempo_change(m_beat, m_bpm);
    if (m_iter_store) {
      *m_iter_store = iter;
      m_iter_store = 0;
    }
    return (iter != m_song.tempo_end());
  }
  
  
  bool AddTempoChange::undo_command() {
    if (m_oldbpm == -1) {
      Song::TempoIterator iter = m_song.tempo_find(m_beat);
      if (iter == m_song.tempo_end())
	return false;
      m_song.remove_tempo_change(iter);
    }
    else
      m_song.add_tempo_change(m_beat, m_oldbpm);
    return true;
  }
  

  RemoveTempoChange::RemoveTempoChange(Song& song, unsigned long beat)
    : Command("Remove tempo change"),
      m_song(song),
      m_beat(beat),
      m_bpm(-1) {

  }
  
  
  bool RemoveTempoChange::do_command() {
    Song::TempoIterator iter = m_song.tempo_find(m_beat);
    if (iter == m_song.tempo_end() || iter->get_beat() != m_beat)
      return false;
    m_bpm = iter->get_bpm();
    m_song.remove_tempo_change(iter);
    return true;
  }
  
  
  bool RemoveTempoChange::undo_command() {
    m_song.add_tempo_change(m_beat, m_bpm);
    return true;
  }


  SetTrackName::SetTrackName(Song& song, int track, const std::string& name)
    : Command("Set track name"),
      m_song(song),
      m_track(track),
      m_name(name) {
    
  }
  
  
  bool SetTrackName::do_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    m_oldname = titer->get_name();
    if (m_oldname == m_name)
      return false;
    titer->set_name(m_name);
    return true;
  }

  
  bool SetTrackName::undo_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    titer->set_name(m_oldname);
    return true;
  }
  

  AddPattern::AddPattern(Song& song, int track, const std::string& name, 
			 int length, int steps, Track::PatternIterator* iter)
    : Command("Add pattern"),
      m_song(song),
      m_track(track),
      m_name(name),
      m_length(length),
      m_steps(steps),
      m_iter_store(iter),
      m_id(-1) {
    
  }
  
  
  bool AddPattern::do_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end()) {
      if (m_iter_store) {
	*m_iter_store = Track::PatternIterator();
	m_iter_store = 0;
      }
      return false;
    }
    Track::PatternIterator iter = titer->add_pattern(m_name, m_length, m_steps);
    if (m_iter_store) {
      *m_iter_store = iter;
      m_iter_store = 0;
    }
    if (iter != titer->pat_end()) {
      m_id = iter->get_id();
      return true;
    }
    return false;
  }
  
  
  bool AddPattern::undo_command() {
    if (m_id == -1)
      return false;
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    Track::PatternIterator piter = titer->pat_find(m_id);
    if (piter == titer->pat_end())
      return false;
    titer->remove_pattern(m_id);
    return true;
  }


  DuplicatePattern::DuplicatePattern(Song& song, int track, int pattern,
				     Track::PatternIterator* iter)
    : Command("Duplicate pattern"),
      m_song(song),
      m_track(track),
      m_pattern(pattern),
      m_iter_store(iter),
      m_id(-1) {
    
  }
  
  
  bool DuplicatePattern::do_command() {
    // XXX this can be written with less code
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end()) {
      if (m_iter_store) {
	*m_iter_store = Track::PatternIterator();
	m_iter_store = 0;
      }
      return false;
    }
    Track::PatternIterator src = titer->pat_find(m_pattern);
    if (src == titer->pat_end()) {
      if (m_iter_store) {
	*m_iter_store = Track::PatternIterator();
	m_iter_store = 0;
      }
      return false;
    }
    Track::PatternIterator iter = titer->duplicate_pattern(src);
    if (m_iter_store) {
      *m_iter_store = iter;
      m_iter_store = 0;
    }
    if (iter != titer->pat_end()) {
      m_id = iter->get_id();
      return true;
    }
    return false;
  }
  
  
  bool DuplicatePattern::undo_command() {
    if (m_id == -1)
      return false;
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    Track::PatternIterator piter = titer->pat_find(m_id);
    if (piter == titer->pat_end())
      return false;
    titer->remove_pattern(m_id);
    return true;
  }


  RemovePattern::RemovePattern(Song& song, int track, int pattern)
    : CompoundCommand("Remove pattern"),
      m_song(song),
      m_track(track),
      m_pattern(pattern) {

  }
  
  
  bool RemovePattern::do_command() {
    
    clear();
    
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    Track::PatternIterator piter = titer->pat_find(m_pattern);
    if (piter == titer->pat_end())
      return false;
    
    // first remove all sequence entries for this pattern (in an undoable way)
    Track::SequenceIterator siter;
    for (siter = titer->seq_begin(); siter != titer->seq_end(); ++siter) {
      if (siter->get_pattern_id() == m_pattern)
	append(new RemoveSequenceEntry(m_song, m_track, siter->get_start()));
    }
    
    append(new _RemovePattern(m_song, m_track, m_pattern));
    
    return CompoundCommand::do_command();
  }
  
  
  RemoveSequenceEntry::RemoveSequenceEntry(Song& song, int track, 
					   unsigned long beat)
    : Command("Remove sequence entry"),
      m_song(song),
      m_track(track),
      m_beat(beat),
      m_pattern(-1),
      m_length(-1) {

  }
  
  
  bool RemoveSequenceEntry::do_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    Track::SequenceIterator siter = titer->seq_find(m_beat);
    if (siter == titer->seq_end())
      return false;
    m_beat = siter->get_start();
    m_pattern = siter->get_pattern_id();
    m_length = siter->get_length();
    titer->remove_sequence_entry(siter);
    return true;
  }
  
  
  bool RemoveSequenceEntry::undo_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    titer->set_sequence_entry(m_beat, m_pattern, m_length);
    return true;
  }


  SetSequenceEntryLength::SetSequenceEntryLength(Song& song, int track, 
						 unsigned long beat, 
						 unsigned int length)
    : Command("Change sequence entry length"),
      m_song(song),
      m_track(track),
      m_beat(beat),
      m_length(length),
      m_old_length(-1) {

  }
  
  
  bool SetSequenceEntryLength::do_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    Track::SequenceIterator siter = titer->seq_find(m_beat);
    if (siter == titer->seq_end())
      return false;
    m_beat = siter->get_start();
    m_old_length = siter->get_length();
    if (m_old_length == m_length)
      return false;
    titer->set_seq_entry_length(siter, m_length);
    return true;
  }

  
  bool SetSequenceEntryLength::undo_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    Track::SequenceIterator siter = titer->seq_find(m_beat);
    if (siter == titer->seq_end())
      return false;
    titer->set_seq_entry_length(siter, m_old_length);
    return true;
  }


  SetTrackMidiChannel::SetTrackMidiChannel(Song& song, int track, int channel)
    : Command("Change track MIDI channel"),
      m_song(song),
      m_track(track),
      m_channel(channel),
      m_old_channel(-1) {

  }

  
  bool SetTrackMidiChannel::do_command() {
    if (m_channel < 0 || m_channel >= 16)
      return false;
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    m_old_channel = titer->get_channel();
    titer->set_channel(m_channel);
    return true;
  }
  
  
  bool SetTrackMidiChannel::undo_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    titer->set_channel(m_old_channel);
    return true;
  }


  AddController::AddController(Song& song, int track, long number, 
			       const std::string& name, int default_v, 
			       int min, int max, bool global)
    : Command("Add controller"),
      m_song(song),
      m_track(track),
      m_number(number),
      m_name(name),
      m_default(default_v),
      m_min(min),
      m_max(max),
      m_global(global) {

  }
  
  
  bool AddController::do_command() {
    if (!is_cc(m_number) && !is_pbend(m_number))
      return false;
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    return titer->add_controller(m_number, m_name, m_default, 
				 m_min, m_max, m_global);
  }
  

  bool AddController::undo_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    return titer->remove_controller(m_number);
  }


  RemoveController::RemoveController(Song& song, int track, long number) 
    : Command("Remove controller"),
      m_song(song),
      m_track(track),
      m_number(number),
      m_info(0) {

  }
  
  
  RemoveController::~RemoveController() {
    if (m_info)
      Deleter::queue(m_info);
    std::map<int, Curve*>::iterator iter;
    for (iter = m_curves.begin(); iter != m_curves.end(); ++iter)
      Deleter::queue(iter->second);
  }
  
  
  bool RemoveController::do_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    m_info = titer->disown_controller(m_number, m_curves);
    if (!m_info)
      return false;
  }
  

  bool RemoveController::undo_command() {
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    if (titer == m_song.tracks_end())
      return false;
    bool result = titer->add_controller(m_info, m_curves);
    m_curves.clear();
    return result;
  }
  

}
