#ifndef SONG_HPP
#define SONG_HPP

#include <map>
#include <string>

#include <sigc++/signal.h>

#include "tempomap.hpp"
#include "track.hpp"


using namespace Glib;
using namespace sigc;
using namespace std;


/** This class contains information about tracks and tempo changes in 
    a song, and some metadata like title, author, and comments. */
class Song {
public:
  
  Song();
  
  // accessors
  const string& get_title() const;
  const string& get_author() const;
  const string& get_info() const;
  const map<int, Track*>& get_tracks() const;
  map<int, Track*>& get_tracks();
  int get_length() const;
  const TempoMap::TempoChange* get_tempo_changes() const;
  
  // mutators
  void set_title(const string& title);
  void set_author(const string& author);
  void set_info(const string& info);
  int add_track(const string& name = "");
  bool remove_track(int id);
  void add_tempo_change(int beat, double bpm);
  void remove_tempo_change(int beat);
  
  // sequencing
  void get_timebase_info(unsigned long frame, unsigned long framerate,
			 double& bpm, int32_t& beat, int32_t& tick) const;
  double get_current_tempo(int beat, int tick);
  unsigned long bt2frame(int beat, int tick);
  pair<int, int> frame2bt(unsigned long frame);
  
  // XML I/O
  bool is_dirty() const;
  bool write_file(const string& filename) const;
  bool load_file(const string& filename);
  void clear();
  
public:
  
  mutable signal<void, const string&> signal_title_changed;
  mutable signal<void, const string&> signal_author_changed;
  mutable signal<void, const string&> signal_info_changed;
  mutable signal<void, int> signal_length_changed;
  mutable signal<void, int> signal_track_added;
  mutable signal<void, int> signal_track_removed;
  mutable signal<void> signal_tempo_changed;
  
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


#endif
