#ifndef SONG_HPP
#define SONG_HPP

#include <map>
#include <string>

#include "track.hpp"


using namespace std;


class Song {
public:
  Song();
  
  void set_title(const string& title);
  void set_author(const string& author);
  void set_info(const string& info);
  int add_track();
  bool remove_track(int id);
  
  const string& get_title() const;
  const string& get_author() const;
  const string& get_info() const;
  const map<int, Track>& get_tracks() const;
  map<int, Track>& get_tracks();
  int get_length() const;
  
  bool is_dirty() const;
  
  const bool write_file(const string& filename) const;
  
private:
  
  string m_title;
  string m_author;
  string m_info;
  map<int, Track> m_tracks;
  int m_length;
  
  mutable bool m_dirty;
};


#endif
