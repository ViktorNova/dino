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

#include "sequencable.hpp"


namespace Dino {
  
  
  using std::string;
  using std::unique_ptr;
  
  
  Sequencable::Position::Position(SongTime const& st)
    : m_time(st) {
  }

  
  SongTime const& Sequencable::Position::get_time() const throw() {
    return m_time;
  }
    
  
  /** Create a new Sequencable object with the given label and length. */
  Sequencable::Sequencable(string const label, SongTime const& length)
    : m_label(label),
      m_length(length) {
  }
    
  
  unique_ptr<Sequencable::Position> 
  Sequencable::create_position(SongTime const& st) const {
    return unique_ptr<Position>(new Position(st));
  }
  
  
  void Sequencable::update_position(Position& pos, SongTime const& st) const {
    pos.m_time = st;
  }
  
  
  string const& Sequencable::get_label() const throw() {
    return m_label;
  }
    
  
  SongTime const& Sequencable::get_length() const throw() {
    return m_length;
  }
    
  
  void Sequencable::set_label(string const& label) {
    m_label = label;
  }
  
  
  void Sequencable::set_length(SongTime const& st) {
    m_length = st;
  }


}

