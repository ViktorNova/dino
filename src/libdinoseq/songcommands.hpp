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
#include "notecollection.hpp"
#include "song.hpp"
#include "track.hpp"


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
  
  
  class AddTempoChange : public Command {
  public:
    AddTempoChange(Song& song, int beat, double bpm, Song::TempoIterator* iter);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_beat;
    double m_bpm;
    int m_oldbpm;
    Song::TempoIterator* m_iter_store;
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
  

  class SetTrackName : public Command {
  public:
    SetTrackName(Song& song, int track, const std::string& name);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    std::string m_name;
    std::string m_oldname;
  };
  

  class AddPattern : public Command {
  public:
    AddPattern(Song& song, int track, const std::string& name, 
	       int length, int steps, Track::PatternIterator* iter);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    std::string m_name;
    int m_length;
    int m_steps;
    Track::PatternIterator* m_iter_store;
    int m_id;
  };
  

  class DuplicatePattern : public Command {
  public:
    DuplicatePattern(Song& song, int track, int pattern, 
		     Track::PatternIterator* iter);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
    Track::PatternIterator* m_iter_store;
    int m_id;
  };
  

  class RemovePattern : public CompoundCommand {
  public:
    RemovePattern(Song& song, int track, int pattern);
    bool do_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
  };
  
  
  class AddSequenceEntry : public Command {
  public:
    AddSequenceEntry(Song& song, int track, int beat, int pattern, int length);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_beat;
    int m_pattern;
    int m_length;
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
  
  
  class SetTrackMidiChannel : public Command {
  public:
    SetTrackMidiChannel(Song& song, int track, int channel);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_channel;
    int m_old_channel;
  };
  
  
  class AddController : public Command {
  public:
    AddController(Song& song, int track, long number, const std::string& name,
		  int default_v, int min, int max, bool global);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    long m_number;
    const std::string& m_name;
    int m_default;
    int m_min;
    int m_max;
    bool m_global;
  };
  

  class RemoveController : public Command {
  public:
    RemoveController(Song& song, int track, long number);
    ~RemoveController();
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    long m_number;
    ControllerInfo* m_info;
    std::map<int, Curve*> m_curves;
  };


  class SetControllerName : public Command {
  public:
    SetControllerName(Song& song, int track, long number, 
		      const std::string& name);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    long m_number;
    std::string m_name;
    std::string m_old_name;
  };


  class SetControllerMin : public Command {
  public:
    SetControllerMin(Song& song, int track, long number, int min);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    long m_number;
    int m_min;
    int m_oldmin;
  };
  
  
  class SetControllerMax : public Command {
  public:
    SetControllerMax(Song& song, int track, long number, int max);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    long m_number;
    int m_max;
    int m_oldmax;
  };
  
  
  class SetControllerDefault : public Command {
  public:
    SetControllerDefault(Song& song, int track, long number, int _default);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    long m_number;
    int m_default;
    int m_olddefault;
  };
  
  
  class SetControllerNumber : public Command {
  public:
    SetControllerNumber(Song& song, int track, long number, long number);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    long m_number;
    int m_newnumber;
  };
  
  
  class SetControllerGlobal : public Command {
  public:
    SetControllerGlobal(Song& song, int track, long number, bool global);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    long m_number;
    bool m_global;
    bool m_oldglobal;
  };


  class AddKey : public Command {
  public:
    AddKey(Song& song, int track, unsigned char number, 
	   const std::string& name);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    unsigned char m_number;
    std::string m_name;
  };


  class RemoveKey : public Command {
  public:
    RemoveKey(Song& song, int track, unsigned char number);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    unsigned char m_number;
    std::string m_name;
  };


  class SetKeyName : public Command {
  public:
    SetKeyName(Song& song, int track, unsigned char number, 
	      const std::string& name);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    unsigned char m_number;
    std::string m_name;
    std::string m_oldname;
  };


  class SetKeyNumber : public Command {
  public:
    SetKeyNumber(Song& song, int track, unsigned char old_number, 
	      unsigned char new_number);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    unsigned char m_newnumber;
    unsigned char m_oldnumber;
  };


  class SetPatternName : public Command {
  public:
    SetPatternName(Song& song, int track, int pattern, const std::string& name);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
    std::string m_name;
    std::string m_oldname;
  };


  class SetPatternLength : public CompoundCommand {
  public:
    SetPatternLength(Song& song, int track, int pattern, unsigned int beats);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
    unsigned int m_beats;
    unsigned int m_oldbeats;
  };
  
  
  class SetPatternSteps : public Command {
  public:
    SetPatternSteps(Song& song, int track, int pattern, unsigned int steps);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
    unsigned int m_steps;
    unsigned int m_oldsteps;
    NoteCollection m_notes;
    unsigned int m_step;
    int m_key;
  };


  class AddNote : public Command {
  public:
    AddNote(Song& song, int track, int pattern, 
	    unsigned int step, int key, int velocity, int length);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
    unsigned int m_step;
    int m_key;
    int m_velocity;
    int m_length;
  };


  class AddNotes : public Command {
  public:
    AddNotes(Song& song, int track, int pattern, const NoteCollection& notes, 
	     unsigned step, int key, NoteSelection* selection);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
    NoteCollection m_notes;
    unsigned m_step;
    int m_key;
    NoteSelection* m_selection;
  };
  

  class SetNoteVelocity : public Command {
  public:
    SetNoteVelocity(Song& song, int track, int pattern, int step, int key,
		    int velocity);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
    int m_step;
    int m_key;
    int m_velocity;
    int m_oldvelocity;
  };

  
  class SetNoteSize : public Command {
  public:
    SetNoteSize(Song& song, int track, int pattern, int step, int key,
		int size);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
    int m_step;
    int m_key;
    int m_size;
    int m_oldsize;
  };
  
  
  class DeleteNote : public Command {
  public:
    DeleteNote(Song& song, int track, int pattern, int step, int key);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
    int m_step;
    int m_key;
    int m_velocity;
    int m_length;
  };


  class AddPatternCurvePoint : public Command {
  public:
    AddPatternCurvePoint(Song& song, int track, int pattern, long number,
			 unsigned int step, int value);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
    long m_number;
    unsigned int m_step;
    int m_value;
    int m_oldvalue;
    bool m_wasold;
  };


  class AddTrackCurvePoint : public Command {
  public:
    AddTrackCurvePoint(Song& song, int track, long number,
		       unsigned int step, int value);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    long m_number;
    unsigned int m_step;
    int m_value;
    int m_oldvalue;
    bool m_wasold;
  };


  class RemovePatternCurvePoint : public Command {
  public:
    RemovePatternCurvePoint(Song& song, int track, int pattern, long number,
			    unsigned int step);
    bool do_command();
    bool undo_command();
  protected:
    Song& m_song;
    int m_track;
    int m_pattern;
    long m_number;
    unsigned int m_step;
    int m_oldvalue;
  };


}


#endif
