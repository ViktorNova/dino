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

#include "deleter.hpp"
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
    if (titer == m_song.tracks_end())
      return false;
    m_id = titer->get_id();
    if (m_iter_store) {
      *m_iter_store = titer;
      m_iter_store = 0;
    }
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
  

}
