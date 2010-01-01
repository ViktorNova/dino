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

#include "songtime.hpp"


namespace Dino {
  
  
  SongTime::SongTime() throw()
    : m_data(0) {
  }
  
  
  SongTime::SongTime(Beat beat, Tick tick) throw()
    : m_data((static_cast<int64_t>(beat) * (1 << 24)) | (tick & 0xFFFFFF)) {
  }
  
  
  bool SongTime::operator==(SongTime const& st) const throw() {
    return m_data == st.m_data;
  }
  
  
  bool SongTime::operator!=(SongTime const& st) const throw() {
    return !operator==(st);
  }
    
  
  bool SongTime::operator<(SongTime const& st) const throw() {
    return m_data < st.m_data;
  }
  
  
  bool SongTime::operator>(SongTime const& st) const throw() {
    return m_data > st.m_data;
  }
  
  
  bool SongTime::operator<=(SongTime const& st) const throw() {
    return !operator>(st);
  }
  
  
  bool SongTime::operator>=(SongTime const& st) const throw() {
    return !operator<(st);
  }
  

  SongTime SongTime::operator+(SongTime const& st) const throw() {
    return SongTime(m_data + st.m_data);
  }

  
  SongTime SongTime::operator-(SongTime const& st) const throw() {
    return SongTime(m_data - st.m_data);
  }
  
  
  SongTime& SongTime::operator+=(SongTime const& st) throw() {
    m_data += st.m_data;
    return *this;
  }
  
  
  SongTime& SongTime::operator-=(SongTime const& st) throw() {
    m_data -= st.m_data;
    return *this;
  }
  
  
  SongTime::Beat SongTime::get_beat() const throw() {
    return m_data / (1 << 24);
  }
  
  
  SongTime::Tick SongTime::get_tick() const throw() {
    return m_data & 0xFFFFFF;
  }


  void SongTime::set_beat(Beat b) throw() {
    m_data = (static_cast<int64_t>(b) * (1 << 24)) | get_tick();
  }

  
  void SongTime::set_tick(Tick t) throw() {
    m_data = (get_beat() * (1 << 24)) | (t & 0xFFFFFF);
  }


  SongTime::SongTime(int64_t data) throw()
    : m_data(data) {
  }


  std::ostream& operator<<(std::ostream& os, SongTime const& st) {
    auto f = os.flags();
    os<<std::hex<<std::uppercase<<st.get_beat()<<':'
      <<std::setw(6)<<std::setfill('0')<<st.get_tick();
    os.flags(f);
    return os;
  }


  SongTime::Tick SongTime::ticks_per_beat() throw() {
    return (1 << 24) - 1;
  }


  unsigned SongTime::overhead_bits() throw() {
    return 16;
  }


  SongTime SongTime::max_valid() throw() {
    return SongTime(int64_t(1) << 47);
  }

}
