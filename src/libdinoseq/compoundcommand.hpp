/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006-2007  Lars Luthman <lars.luthman@gmail.com>
   
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

#ifndef COMPOUNDCOMMAND_HPP
#define COMPOUNDCOMMAND_HPP

#include <vector>

#include "command.hpp"


namespace Dino {
  
  
  class CompoundCommand : public Command {
  public:
    
    CompoundCommand(const std::string& name);
    ~CompoundCommand();
    
    bool do_command();
    bool undo_command();
    
    bool append(Command* cmd);
    
  protected:
    
    std::vector<Command*> m_commands;
    
    bool m_used;
    
  };
  

}


#endif
