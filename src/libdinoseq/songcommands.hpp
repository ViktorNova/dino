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

#ifndef SONGCOMMANDS_HPP
#define SONGCOMMANDS_HPP

#include <string>

#include "command.hpp"
#include "compoundcommand.hpp"
#include "song.hpp"


namespace Dino {
  
  
  class SetSongLength : public CompoundCommand {
  public:
    SetSongLength(Song& song, int length);
    bool do_command();
  protected:
    Song& m_song;
    int m_length;
  };
  
  
  class SetLoopStart : public Command {
  public:
    SetLoopStart(Song& song, int beat);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_beat;
    int m_oldbeat;
  };
  
  
  class SetLoopEnd : public Command {
  public:
    SetLoopEnd(Song& song, int beat);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_beat;
    int m_oldbeat;
  };


  class AddTrack : public Command {
  public:
    AddTrack(Song& song, const std::string& name, Song::TrackIterator* iter);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    std::string m_name;
    int m_id;
    Song::TrackIterator* m_iter_store;
  };
  
  
  class RemoveTrack : public Command {
  public:
    RemoveTrack(Song& song, int id);
    ~RemoveTrack();
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_id;
    Track* m_trk;
  };
  
  
  class RemoveTempoChange : public Command {
  public:
    RemoveTempoChange(Song& song, unsigned long beat);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    unsigned long m_beat;
    double m_bpm;
  };
  

  class RemoveSequenceEntry : public Command {
  public:
    RemoveSequenceEntry(Song& song, int track, unsigned long beat);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    unsigned long m_beat;
    int m_pattern;
    int m_length;
  };


  class SetSequenceEntryLength : public Command {
  public:
    SetSequenceEntryLength(Song& song, int track, unsigned long beat, 
			   unsigned int length);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    unsigned long m_beat;
    int m_length;
    int m_old_length;
  };
  

}


#endif
