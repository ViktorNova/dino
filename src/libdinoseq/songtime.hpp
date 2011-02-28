/*****************************************************************************
    libdinoseq - a library for MIDI sequencing
    Copyright (C) 2009  Lars Luthman <mail@larsluthman.net>
    
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
  
  
  /** A type that represents time in a Sequencable or a Song. */
  class SongTime {
  public:
    
    /** The type that is used to count beats. */
    typedef int32_t Beat;
    
    /** The type that is used to count ticks within beats. One beat consists
	of ticks_per_beat() ticks. */
    typedef uint32_t Tick;
    
    /** Create a SongTime at beat 0 and tick 0. */
    SongTime() throw();
    
    /** Create a SongTime with the given beat and tick. */
    SongTime(Beat beat, Tick tick) throw();
    
    /** Compare for equality. */
    bool operator==(SongTime const& st) const throw();

    /** Compare for inequality. */
    bool operator!=(SongTime const& st) const throw();
    
    /** Return true if this SongTime is earlier than another. */
    bool operator<(SongTime const& st) const throw();

    /** Return true if this SongTime is later than another. */
    bool operator>(SongTime const& st) const throw();

    /** Return true if this SongTime is not later than another. */
    bool operator<=(SongTime const& st) const throw();

    /** Return true if this SongTime is not earlier than another. */
    bool operator>=(SongTime const& st) const throw();
    
    /** Add one SongTime to another, return the result by value. */
    SongTime operator+(SongTime const& st) const throw();
    
    /** Subtract one SongTime from another, return the result by value. */
    SongTime operator-(SongTime const& st) const throw();
    
    /** Add SongTime objects in place. */
    SongTime& operator+=(SongTime const& st) throw();

    /** Subtract SongTime objects in place. */
    SongTime& operator-=(SongTime const& st) throw();
    
    /** Get the beat. */
    Beat get_beat() const throw();
    
    /** Get the tick. */
    Tick get_tick() const throw();
    
    /** Set the beat. */
    void set_beat(Beat b) throw();
    
    /** Set the tick. */
    void set_tick(Tick t) throw();
    
    /** Return the number of ticks per beat. */
    static Tick ticks_per_beat() throw();

    /** Return the number of bits of overhead that is available. */
    static unsigned overhead_bits() throw();

    /** Return the maximal length of a song. Because of the overhead this is
	not the maximal value representable by a SongTime. */
    static SongTime max_valid() throw();

  private:
    
    /** Initialise a SongTime object from a single 64 bit integer. */
    SongTime(int64_t data) throw();
    
    /** The actual data. */
    int64_t m_data;
    
  };

  
  /** An ostream operator for SongTime. */
  std::ostream& operator<<(std::ostream& os, SongTime const& st);
  
  
}


#endif
