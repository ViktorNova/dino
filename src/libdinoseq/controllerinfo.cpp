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
      m_default(64),
      m_min(0),
      m_max(127),
      m_name(name),
      m_global(m_type != MIDI_PITCH) {

    if (m_type == MIDI_PITCH) {
      m_default = 8192;
      m_max = 16383;
    }
  }
    

  ControllerType ControllerInfo::get_type() const {
    return m_type;
  }
  
  
  int ControllerInfo::get_default() const {
    return m_default;
  }
  
  
  int ControllerInfo::get_min() const {
    return m_min;
  }
  
  
  int ControllerInfo::get_max() const {
    return m_max;
  }
  
  
  const std::string& ControllerInfo::get_name() const {
    return m_name;
  }
  
  
  bool ControllerInfo::get_global() const {
    return m_global;
  }
  
    
  void ControllerInfo::set_type(ControllerType type) {
    m_type = type;
  }
  
  
  void ControllerInfo::set_default(int def) {
    m_default = def;
  }
  
  
  void ControllerInfo::set_min(int min) {
    m_min = min;
  }
  

  void ControllerInfo::set_max(int max) {
    m_max = max;
  }
  

  void ControllerInfo::set_name(const std::string& name) {
    m_name = name;
  }
  
  
  void ControllerInfo::set_global(bool global) {
    m_global = global;
  }
  
  
}

