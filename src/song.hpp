#ifndef SONG_HPP
#define SONG_HPP

#include <map>
#include <string>

#include "track.hpp"


using namespace std;


class Song {
public:
  Song();
  
  void setTitle(const string& title);
  void setAuthor(const string& author);
  void setInfo(const string& info);
  int addTrack();
  
  const string& getTitle() const;
  const string& getAuthor() const;
  const string& getInfo() const;
  const map<int, Track>& getTracks() const;
  map<int, Track>& getTracks();
  int getLength() const;
  
  bool isDirty() const;
  
  const bool writeFile(const string& filename) const;
  
private:
  
  string mTitle;
  string mAuthor;
  string mInfo;
  map<int, Track> mTracks;
  int mLength;
  
  mutable bool mDirty;
};


#endif
