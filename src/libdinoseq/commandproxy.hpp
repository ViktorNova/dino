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

#include <string>
#include <stack>

#include <sigc++/signal.h>

#include "song.hpp"
#include "track.hpp"


namespace Dino {
  
  
  class CompoundCommand;
  class Command;
  class NoteCollection;
  class NoteSelection;
  
  
  /** A proxy class that provides a single interface for modifying the
      song and controlling the sequencer, as well as an Undo layer.
      None of the command functions or undo functions defined in this class
      should be called recursively or mutually recursively, i.e. only one
      of them may be executing at any given time. If any of the functions
      are called while another one is executing (for example because it has
      been connected to a signal that is fired by the first function), the 
      second one will fail. */
  class CommandProxy {
  public:
    
    /** Creates a new command proxy for the given Song and Sequencer object.
	The Song and Sequencer must not be destroyed before this CommandProxy
	is. */
    CommandProxy(Song& song);
    
    /** Return @c true if the latest command can be undone. */
    bool can_undo() const;
    
    /** Return the name of the undoable command (or an empty string if there
	is none). */
    std::string get_next_undo_name() const;
    
    /** Undo the last command, if possible. */
    bool undo();

    /** Start a new atomic command - queue subsequent command calls and don't
	execute them until end_atomic() is called. If undo() is called before
	end_atomic() all queued commands will be discarded. start_atomic()
	may be called multiple times before end_atomic() is called, in which
	case end_atomic() must be called the same number of times to end the
	atomic command. */
    bool start_atomic(const std::string& name);
    
    /** End an atomic command - execute all queued command calls. */
    bool end_atomic();
    
    /** Return a constant reference to the song. */
    const Song& get_song() const;
    
    
    /// @name Song commands
    //@{
    
    /** Set the title of the song. */
    bool set_song_title(const std::string& title);
    /** Set the song author's name. */
    bool set_song_author(const std::string& author);
    /** Set the information for the song. */
    bool set_song_info(const std::string& info);
    /** Set the song length. */
    bool set_song_length(int length);
    /** Set the loop start. */
    bool set_loop_start(int beat);
    /** Set the loop end. */
    bool set_loop_end(int beat);
    /** Add a new track and optionally store its iterator in @c iter. */
    bool add_track(const std::string& name, Song::TrackIterator* iter = 0);
    /** Remove a track. */
    bool remove_track(int id);
    /** Add a tempo change. */
    bool add_tempo_change(int beat, double bpm, Song::TempoIterator* iter = 0);
    /** Remove a tempo change. */
    bool remove_tempo_change(unsigned long beat);

    
    /** Set the name for a track. */
    bool set_track_name(int id, const std::string& name);
    /** Add a pattern to a track. */
    bool add_pattern(int track, const std::string& name, int length, int steps,
		     Track::PatternIterator* iter = 0);
    /** Duplicate an existing pattern. */
    bool duplicate_pattern(int track, int pattern, 
			   Track::PatternIterator* iter = 0);
    /** Remove an existing pattern. */
    bool remove_pattern(int track, int pattern);
    /** Add a sequence entry. */
    bool add_sequence_entry(int track, int beat, int pattern, int length = 0);
    /** Remove a sequence entry. */
    bool remove_sequence_entry(int track, unsigned long beat);
    /** Change the length of a sequence entry. */
    bool set_sequence_entry_length(int track, unsigned long beat, 
				   unsigned int length);
    /** Set the MIDI channel of a track. */
    bool set_track_midi_channel(int track, int channel);
    /** Add a controller for a track. */
    bool add_controller(int track, long number, const std::string& name,
			int default_v, int min, int max, bool global);
    /** Remove a controller for a track. */
    bool remove_controller(int track, long number);
    /** Change the name of a controller. */
    bool set_controller_name(int track, long number, const std::string& name);
    /** Change the minimum value of a controller. */
    bool set_controller_min(int track, long number, int min);
    /** Change the maximum value of a controller. */
    bool set_controller_max(int track, long number, int max);
    /** Change the default value of a controller. */
    bool set_controller_default(int track, long number, int _default);
    /** Change the parameter number value of a controller. */
    bool set_controller_number(int track, long old_number, long new_number);
    /** Change the 'global' toggle of a controller. */
    bool set_controller_global(int track, long number, bool global);
    /** Add a named key for a track. */
    bool add_key(int track, unsigned char number, const std::string& name);
    /** Remove a named key for a track. */
    bool remove_key(int track, unsigned char number);
    /** Change the name of a key. */
    bool set_key_name(int track, unsigned char number, const std::string& name);
    /** Change the key number value of a named key. */
    bool set_key_number(int track, unsigned char old_number, 
			unsigned char new_number);
    /** Set the mode of a track. */
    bool set_track_mode(int track, Track::Mode mode);
    /** Change the name of a pattern. */
    bool set_pattern_name(int track, int pattern, const std::string& name);
    /** Change the length of a pattern (in beats). */
    bool set_pattern_length(int track, int pattern, unsigned int beats);
    /** Change the number of steps per beat in a pattern. */
    bool set_pattern_steps(int track, int pattern, unsigned int steps);
    /** Add a note to a pattern. */
    bool add_note(int track, int pattern, unsigned int step, 
		  int key, int velocity, int length);
    /** Add a collection of notes to a pattern. */
    bool add_notes(int track, int pattern, const NoteCollection& notes, 
		   unsigned step, int key, NoteSelection* selection = 0);
    /** Change the velocity of a note. */
    bool set_note_velocity(int track, int pattern, int step, int key, 
			   int velocity);
    /** Resize a note. */
    bool set_note_size(int track, int pattern, int step, int key, int size);
    /** Delete a note. */
    bool delete_note(int track, int pattern, int step, int key);
    /** Add a point to a pattern curve. */
    bool add_curve_point(int track, int pattern, long number, 
			 unsigned step, int value);
    /** Remove a point from a pattern curve. */
    bool remove_curve_point(int track, int pattern, long number, 
			    unsigned step);
    
    //@}
    
    sigc::signal<void>& signal_stack_changed();
    
  protected:
    
    /** Used internally to push a command onto the undo stack, execute it,
	and fire off the signal_stack_changed() signal. */
    bool push_and_do(Command* cmd);

    /** A reference to the sequencer object this CommandProxy is controlling. */
    //Sequencer& m_seq;
    /** A reference to the song object this CommandProxy is controlling. */
    Song& m_song;
    
    /** The stack of undoable commands. */
    std::stack<Command*> m_stack;
    
    /** A signal that is activated whenever the stack changes. */
    sigc::signal<void> m_signal_stack_changed;
    
    /** A flag that is used internally to prevent recursive commands. */
    bool m_active;
    
    /** A pointer to the current atomic command (NULL if there is no current
	atomic command). */
    CompoundCommand* m_atomic;
    
    /** A counter to allow nested start_atomic()/end_atomic() calls. */
    unsigned m_atomic_count;
    
  };
  
  
}
