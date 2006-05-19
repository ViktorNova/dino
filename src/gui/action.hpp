#ifndef ACTION_HPP
#define ACTION_HPP

#include <string>

#include "song.hpp"


namespace Dino {
  class Track;
  class PatternSelection;
}

using namespace Dino;


struct Action {
  virtual std::string get_name() const = 0;
};


struct SongAction : public Action {
  virtual void run(Song& song) = 0;
};


struct PatternSelectionAction : public Action {
  virtual void run(PatternSelection& selection) = 0;
};


#endif
