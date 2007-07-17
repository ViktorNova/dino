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

#ifndef INSTRUMENTINFO_HPP
#define INSTRUMENTINFO_HPP


#include <string>
#include <vector>

#include "controllerinfo.hpp"


namespace Dino {
  

  /** This class contains information about a writable MIDI port. */
  class InstrumentInfo {
  public:
    
    InstrumentInfo(const std::string& str);
    
    InstrumentInfo(const char* str);
    
    const std::string& get_name() const;
    bool get_connected() const;
    const std::vector<ControllerInfo>& get_controllers() const;
    
    void set_connected(bool connected);
    
  protected:
    
    std::string m_name;
    std::vector<ControllerInfo> m_ctrls;
    bool m_connected;
  };
  

}


#endif
