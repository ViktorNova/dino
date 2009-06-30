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

#include <iomanip>

#include "ostreambuffer.hpp"
#include "songtime.hpp"


namespace Dino {

  
  using namespace std;
  
  
  OStreamBuffer::OStreamBuffer(ostream& stream)
    : m_stream(stream) {
  }
    
  
  bool OStreamBuffer::write_event(SongTime const& st, size_t bytes, 
				  unsigned char const* data) {
    auto f = m_stream.flags();
    m_stream<<st<<':'<<hex<<uppercase;
    for (size_t i = 0; i < bytes; ++i)
      m_stream<<' '<<setw(2)<<setfill('0')<<static_cast<int>(data[i]);
    m_stream<<'\n';
    m_stream.flags(f);
    return true;
  }


}

