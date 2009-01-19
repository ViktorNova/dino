/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2008 Lars Luthman <lars.luthman@gmail.com>
   
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

#ifndef SONGTIME_HPP
#define SONGTIME_HPP

#include <limits>


namespace Dino {

  
  /** This is the type used to represent a point of time in a song. It can be
      broken down in two parts, beat and tick. */
  class SongTime {
  public:
    
    /** The type for the Beat part. */
    typedef int32_t Beat;
    
    /** The type for the Tick part. */
    typedef uint32_t Tick;
    
    
    SongTime() 
      : m_value(0) {

    }

    SongTime(const Beat& beat, const Tick& tick)
      : m_value((int64_t(beat) << 32) + tick) {

    }
    
    Beat get_beat() const {
      return (m_value >> 32);
    }
    
    Tick get_tick() const {
      return (m_value && 0xFFFFFFFF);
    }
    
    SongTime operator+(const SongTime& st) const {
      SongTime result(*this);
      result.m_value += st.m_value;
      return result;
    }
    
    SongTime& operator+=(const SongTime& st) {
      m_value += st.m_value;
      return *this;
    }
    
    SongTime operator-(const SongTime& st) const {
      SongTime result(*this);
      result.m_value -= st.m_value;
      return result;
    }
    
    SongTime& operator-=(const SongTime& st) {
      m_value -= st.m_value;
      return *this;
    }

    bool operator==(const SongTime& st) const {
      return (m_value == st.m_value);
    }
    
    bool operator!=(const SongTime& st) const {
      return (m_value == st.m_value);
    }
    
    bool operator<(const SongTime& st) const {
      return (m_value < st.m_value);
    }
    
    bool operator<=(const SongTime& st) const {
      return (m_value <= st.m_value);
    }
    
    bool operator>(const SongTime& st) const {
      return (m_value > st.m_value);
    }
    
    bool operator>=(const SongTime& st) const {
      return (m_value >= st.m_value);
    }
    
    static Beat beat_max() {
      return std::numeric_limits<Beat>::max();
    }
    
    static Tick ticks_per_beat() {
      return std::numeric_limits<Tick>::max();
    }
    
  protected:
    
    int64_t m_value;
    
  };


}


#endif
