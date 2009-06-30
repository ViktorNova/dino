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

#ifndef SONGTIME_HPP
#define SONGTIME_HPP

#include <iostream>

#include <stdint.h>


namespace Dino {
  
  
  class SongTime {
  public:
    
    typedef int32_t Beat;
    
    typedef uint32_t Tick;
    
    SongTime() throw();
    
    SongTime(Beat beat, Tick tick) throw();
    
    bool operator==(SongTime const& st) const throw();

    bool operator!=(SongTime const& st) const throw();
    
    bool operator<(SongTime const& st) const throw();

    bool operator>(SongTime const& st) const throw();

    bool operator<=(SongTime const& st) const throw();

    bool operator>=(SongTime const& st) const throw();

    SongTime operator+(SongTime const& st) const throw();

    SongTime operator-(SongTime const& st) const throw();
    
    SongTime& operator+=(SongTime const& st) throw();

    SongTime& operator-=(SongTime const& st) throw();
    
    Beat get_beat() const throw();
    
    Tick get_tick() const throw();
    
    void set_beat(Beat b) throw();

    void set_tick(Tick t) throw();
    
  private:
    
    SongTime(int64_t data) throw();
    
    int64_t m_data;
    
  };


  std::ostream& operator<<(std::ostream& os, SongTime const& st);
  
  
}


#endif
