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
    
    class TrackIterator {
    public:      
      const Track& operator*() const { return *(m_iterator->second); }
      Track& operator*() { return *(m_iterator->second); }
      const Track* operator->() const { return m_iterator->second; }
      Track* operator->() { return m_iterator->second; }
      bool operator==(const TrackIterator& iter) const { 
	return (m_iterator == iter.m_iterator);
      }
      bool operator!=(const TrackIterator& iter) const { 
	return (m_iterator != iter.m_iterator);
      }
      
      TrackIterator& operator++() { ++m_iterator; return *this; }
      
    private:
      
      friend class Song;
      friend class ConstTrackIterator;
      
      TrackIterator(const std::map<int, Track*>::iterator& iter)
	: m_iterator(iter) {
      }
      
      std::map<int, Track*>::iterator m_iterator;
    };


    class ConstTrackIterator {
    public:
      ConstTrackIterator() { }
      ConstTrackIterator(const TrackIterator& iter) 
	: m_iterator(iter.m_iterator) {
      }
      
      const Track& operator*() const { return *(m_iterator->second); }
      const Track* operator->() const { return m_iterator->second; }
      bool operator==(const ConstTrackIterator& iter) const { 
	return (m_iterator == iter.m_iterator);
      }
      bool operator!=(const ConstTrackIterator& iter) const { 
	return (m_iterator != iter.m_iterator);
      }
      
      ConstTrackIterator& operator++() { ++m_iterator; return *this; }
      
    private:
      
      friend class Song;
      
      ConstTrackIterator(const std::map<int, Track*>::const_iterator& iter)
	: m_iterator(iter) {
      }
      
      std::map<int, Track*>::const_iterator m_iterator;
    };
    
    
    Song();
    ~Song();
  
    // accessors
    const string& get_title() const;
    const string& get_author() const;
    const string& get_info() const;
    ConstTrackIterator tracks_begin() const;
    ConstTrackIterator tracks_end() const;
    ConstTrackIterator find_track(int id) const;
    const size_t get_number_of_tracks() const;
    int get_length() const;
    const TempoMap::TempoChange* get_tempo_changes() const;
    
    // non-const accessors
    TrackIterator tracks_begin();
    TrackIterator tracks_end();
    TrackIterator find_track(int id);

    // mutators
    void set_title(const string& title);
    void set_author(const string& author);
    void set_info(const string& info);
    void set_length(int length);
    TrackIterator add_track(const string& name = "");
    bool remove_track(const TrackIterator& iterator);
    void add_tempo_change(int beat, double bpm);
    void remove_tempo_change(int beat);
  
    // sequencing
    void get_timebase_info(unsigned long frame, unsigned long frame_rate,
			   double& bpm, int32_t& beat, int32_t& tick) const;
    double get_current_tempo(int beat, int tick);
    unsigned long bt2frame(double);
    pair<int, int> frame2bt(unsigned long frame);
  
    // XML I/O
    bool is_dirty() const;
    bool write_file(const string& filename) const;
    bool load_file(const string& filename);
    void clear();
  
  public:
  
    mutable sigc::signal<void, const string&> signal_title_changed;
    mutable sigc::signal<void, const string&> signal_author_changed;
    mutable sigc::signal<void, const string&> signal_info_changed;
    mutable sigc::signal<void, int> signal_length_changed;
    mutable sigc::signal<void, int> signal_track_added;
    mutable sigc::signal<void, int> signal_track_removed;
    mutable sigc::signal<void> signal_tempo_changed;
  
  private:
  
    // non-copyable for now
    Song& operator=(const Song&) { return *this; }
    Song(const Song&) { }
  
    string m_title;
    string m_author;
    string m_info;
    map<int, Track*> m_tracks;
    int m_length;
  
    TempoMap m_tempo_map;
  
    mutable bool m_dirty;
  };

}


#endif
