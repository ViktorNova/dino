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

#include "keyinfo.hpp"


namespace Dino {
  
  
  KeyInfo::KeyInfo(unsigned char number, const std::string& name)
    : m_number(number < 128 ? number : 255),
      m_name(name) {

  }
  
  
  unsigned char KeyInfo::get_number() const {
    return m_number;
  }
  
  
  const std::string& KeyInfo::get_name() const {
    return m_name;
  }
    
  
  void KeyInfo::set_number(unsigned char n) {
    m_number = n < 128 ? n : 255;
  }
  
  
  void KeyInfo::set_name(const std::string& name) {
    m_name = name;
  }


}
