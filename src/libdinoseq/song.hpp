#ifndef SONG_HPP
#define SONG_HPP

#include <map>
#include <string>

#include <glibmm/thread.h>
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
  
  /** This struct contains information about a tempo change in the song. */
  struct TempoChange {
    TempoChange(double new_time, double new_bpm) 
      : time(new_time), bpm(new_bpm), prev(NULL), next(NULL) { }
    double time;
    double bpm;
    TempoChange* prev;
    TempoChange* next;
  };
  
  Song();
  
  // accessors
  const string& get_title() const;
  const string& get_author() const;
  const string& get_info() const;
  const map<int, Track*>& get_tracks() const;
  map<int, Track*>& get_tracks();
  int get_length() const;
  TempoChange* get_tempo_changes();
  
  // mutators
  void set_title(const string& title);
  void set_author(const string& author);
  void set_info(const string& info);
  int add_track(const string& name = "");
  bool remove_track(int id);
  
  // sequencing
  void get_timebase_info(unsigned long frame, unsigned long framerate,
			 double& bpm, int32_t& beat, int32_t& tick) const;
  double get_current_tempo(int beat, int tick);
  //void locate(double second, int& beat, int& tick);
  double get_second(int beat, int tick);
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
  
private:
  
  // non-copyable for now
  Song& operator=(const Song&) { return *this; }
  
  string m_title;
  string m_author;
  string m_info;
  map<int, Track*> m_tracks;
  int m_length;
  TempoChange* m_tempo_head;
  
  mutable const TempoChange* m_current_tempo;
  
  TempoMap m_tempo_map;
  
  mutable bool m_dirty;
};


#endif
