/*****************************************************************************
    libdinoseq - a library for MIDI sequencing
    Copyright (C) 2009  Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef OSTREAMBUFFER_HPP
#define OSTREAMBUFFER_HPP

#include <iostream>

#include "eventbuffer.hpp"


namespace Dino {


  class SongTime;

  
  /** An EventBuffer that prints the events it receives to an ostream.
      It can be useful for debugging. 
  
      @ingroup sequencing 
  */
  class OStreamBuffer : public EventBuffer {
  public:
    
    /** Create a new OStreamBuffer for a given @c ostream. You can for
	example do
	@code
	OStreamBuffer osb(std::cout);
	@endcode
	to create an OStreamBuffer that will print all events to stdout. 
    */
    OStreamBuffer(std::ostream& stream);
    
    bool write_event(SongTime const& st, size_t bytes, 
		     unsigned char const* data);
    
  private:
    
    /** The @c ostream that events will be written to. */
    std::ostream& m_stream;
    
  };


}


#endif
