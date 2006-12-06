/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include "controllerinfo.hpp"


namespace Dino {
  
  
  ControllerInfo::ControllerInfo(const ControllerType& type, 
                                 const std::string& name)
    : m_type(type),
      m_default(0),
      m_name(name) {
    if (m_type == MIDI_CC)
      m_default = 64;
    else if (m_type == MIDI_PITCH)
      m_default = 8192;
  }
    

  ControllerType ControllerInfo::get_type() const {
    return m_type;
  }
  
  
  int ControllerInfo::get_default() const {
    return m_default;
  }
  
  
  const std::string& ControllerInfo::get_name() const {
    return m_name;
  }
  
    
  void ControllerInfo::set_type(ControllerType type) {
    m_type = type;
  }
  
  
  void ControllerInfo::set_default(int default) {
    m_default = default;
  }
  
  
  void ControllerInfo::set_name(const std::string& name) {
    m_name = name;
  }

}

