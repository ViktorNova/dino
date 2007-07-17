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

#include "instrumentinfo.hpp"


namespace Dino {
  

  InstrumentInfo::InstrumentInfo(const std::string& str)
    : m_name(str),
      m_connected(false) {

  }
  
  
  InstrumentInfo::InstrumentInfo(const char* str)
    : m_name(str),
      m_connected(false) {

  }


  const std::string& InstrumentInfo::get_name() const {
    return m_name;
  }
  
  
  bool InstrumentInfo::get_connected() const {
    return m_connected;
  }
  
  
  const std::vector<ControllerInfo>& InstrumentInfo::get_controllers() const {
    return m_ctrls;
  }
  
  
  void InstrumentInfo::set_connected(bool connected) {
    m_connected = connected;
  }
    

}
