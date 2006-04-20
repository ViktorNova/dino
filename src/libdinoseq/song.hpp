/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <larsl@users.sourceforge.net>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#ifndef SONG_HPP
#define SONG_HPP

#include <map>
#include <string>

#include <sigc++/signal.h>

#include "tempomap.hpp"


using namespace std;


/** This namespace contains front-end independent code for modifying
    and sequencing songs, as well as some utility classes and functions. */
namespace Dino {

  
  class Track;
  

  /** This class contains information about tracks and tempo changes in 
      a song, and some metadata like title, author, and comments. */
  class Song {
  public:
    
    class ConstTrackIterator;
    
    /** An iterator class that is used to access the tracks in the song. */
    class TrackIterator {
    public:      
      const Track& operator*() const;
      Track& operator*();
      const Track* operator->() const;
      Track* operator->();
      bool operator==(const TrackIterator& iter) const;
      bool operator!=(const TrackIterator& iter) const;      
      TrackIterator& operator++();
    private:
      
      friend class Song;
      friend class ConstTrackIterator;
      
      explicit TrackIterator(const std::map<int, Track*>::iterator& iter);
      
      std::map<int, Track*>::iterator m_iterator;
    };


    /** An iterator class that is used to access the tracks in the song, 
	without changing them. */
    class ConstTrackIterator {
    public:
      ConstTrackIterator();
      ConstTrackIterator(const TrackIterator& iter);
      const Track& operator*() const;
      const Track* operator->() const;
      bool operator==(const ConstTrackIterator& iter) const;
      bool operator!=(const ConstTrackIterator& iter) const;
      ConstTrackIterator& operator++();
      
    private:
      
      friend class Song;

      explicit ConstTrackIterator(const std::map<int, Track*>::const_iterator&
				  iter);
      
      std::map<int, Track*>::const_iterator m_iterator;
    };
    
    
    /** An iterator class that is used to access the tempo changes in the song.
	It is a read-only iterator, you have to use the member functions in 
	Song to modify the tempo map.
	@see Song::add_tempo_change(), Song::remove_tempo_change()
    */
    class TempoIterator {
    public:

      TempoIterator();
      const TempoMap::TempoChange& operator*() const;
      const TempoMap::TempoChange* operator->() const;
      bool operator==(const TempoIterator& iter) const;
      bool operator!=(const TempoIterator& iter) const;
      TempoIterator& operator++();
      
    private:
      
      TempoIterator(const TempoMap::TempoChange* tempo);
      
      friend class Song;

      const TempoMap::TempoChange* m_tempo;
    };
    
    
    Song();
    ~Song();
  
    /// @name Accessors
    //@{
    const string& get_title() const;
    const string& get_author() const;
    const string& get_info() const;
    ConstTrackIterator tracks_begin() const;
    ConstTrackIterator tracks_end() const;
    ConstTrackIterator find_track(int id) const;
    TempoIterator tempo_begin() const;
    TempoIterator tempo_end() const;
    TempoIterator tempo_find(int beat) const;
    const size_t get_number_of_tracks() const;
    int get_length() const;
    
    // non-const accessors
    TrackIterator tracks_begin();
    TrackIterator tracks_end();
    TrackIterator find_track(int id);
    //@}

    /// @name Mutators
    //@{
    void set_title(const string& title);
    void set_author(const string& author);
    void set_info(const string& info);
    void set_length(int length);
    TrackIterator add_track(const string& name = "");
    bool remove_track(const TrackIterator& iterator);
    TempoIterator add_tempo_change(int beat, double bpm);
    void remove_tempo_change(TempoIterator& iter);
    //@}
    
    /// @name Sequencing
    //@{
    void get_timebase_info(unsigned long frame, unsigned long frame_rate,
			   double ticks_per_beat, double& bpm, int32_t& beat, 
			   int32_t& tick, double& frame_offset) const;
    double get_current_tempo(int beat, int tick);
    unsigned long bt2frame(double);
    pair<int, int> frame2bt(unsigned long frame);
    //@}
    
    /// @name XML I/O
    //@{
    bool is_dirty() const;
    bool write_file(const string& filename) const;
    bool load_file(const string& filename);
    void clear();
    //@}
    
  public:
    
    /// @name Signals
    //@{
    mutable sigc::signal<void, const string&> signal_title_changed;
    mutable sigc::signal<void, const string&> signal_author_changed;
    mutable sigc::signal<void, const string&> signal_info_changed;
    mutable sigc::signal<void, int> signal_length_changed;
    mutable sigc::signal<void, int> signal_track_added;
    mutable sigc::signal<void, int> signal_track_removed;
    mutable sigc::signal<void> signal_tempo_changed;
    //@}
    
  private:
  
    // non-copyable for now
    Song& operator=(const Song&) { return *this; }
    Song(const Song&) { }
  
    string m_title;
    string m_author;
    string m_info;
    map<int, Track*>* volatile m_tracks;
    volatile int m_length;
  
    TempoMap m_tempo_map;
  
    mutable bool m_dirty;
  };

}


#endif
