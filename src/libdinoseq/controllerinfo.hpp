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

#ifndef CONTROLLERINFO_HPP
#define CONTROLLERINFO_HPP


#include <string>


namespace Dino {
  
  
  class ControllerInfo {
  public:
    
    ControllerInfo(long number, const std::string& name);
    
    long get_number() const;
    int get_default() const;
    int get_min() const;
    int get_max() const;
    const std::string& get_name() const;
    bool get_global() const;
    
    void set_number(long n);
    void set_default(int def);
    void set_min(int min);
    void set_max(int max);
    void set_name(const std::string& name);
    void set_global(bool global);
    
  protected:
    
    long m_number;
    int m_default;
    int m_min;
    int m_max;
    std::string m_name;
    bool m_global;
    
  };


}


#endif
