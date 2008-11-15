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

#ifndef SEQUENCABLE_HPP
#define SEQUENCABLE_HPP

#include <string>


namespace Dino {
  
  
  class MIDIBuffer;
  class SongTime;
  
  
  /** Abstract base class for all objects that generates MIDI events. */
  class Sequencable {
  public:
    
    virtual void sequence(MIDIBuffer& buffer, 
			  const SongTime& from, const SongTime& to,
			  const SongTime& length, int channel) const = 0;
    
    virtual const std::string& get_label() const = 0;

  };


}


#endif
