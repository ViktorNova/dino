/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
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

#ifndef ACTION_HPP
#define ACTION_HPP

#include <string>

#include "pattern.hpp"
#include "song.hpp"
#include "track.hpp"


namespace Dino {
  class CommandProxy;
  class NoteSelection;
}


class Action {
public:
  virtual std::string get_name() const = 0;
};


class SongAction : public Action {
public:
  virtual void run(Dino::CommandProxy& proxy) = 0;
};


class TrackAction : public Action {
public:
  virtual void run(Dino::CommandProxy& proxy, 
		   const Dino::Song::ConstTrackIterator& iter) = 0;
};


class SequenceEntryAction : public Action {
public:
  virtual void run(Dino::CommandProxy& proxy,
		   const Dino::Song::ConstTrackIterator& titer,
		   const Dino::Track::SequenceIterator& siter) = 0;
};


class PatternAction : public Action {
public:
  virtual void run(Dino::CommandProxy& proxy,
		   const Dino::Song::ConstTrackIterator& titer,
		   const Dino::Track::ConstPatternIterator& piter) = 0;
};


class ControllerAction : public Action {
public:
  virtual void run(Dino::CommandProxy& proxy,
		   const Dino::Song::ConstTrackIterator& titer,
		   long number) = 0;
};


class NoteSelectionAction : public Action {
public:
  virtual void run(Dino::CommandProxy& proxy, 
		   const Dino::Song::ConstTrackIterator& titer,
		   const Dino::Track::ConstPatternIterator& piter,
		   Dino::NoteSelection& selection) = 0;
};


#endif
