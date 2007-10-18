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

#include <iostream>

#include "compoundcommand.hpp"
#include "debug.hpp"


namespace Dino {
  
  
  CompoundCommand::CompoundCommand(const std::string& name)
    : Command(name),
      m_used(false) {

  }
  
  
  CompoundCommand::~CompoundCommand() {
    for (unsigned i = 0; i < m_commands.size(); ++i)
      delete m_commands[i];
  }
  
  
  bool CompoundCommand::do_command() {
    m_used = true;
    for (int i = 0; i < static_cast<int>(m_commands.size()); ++i) {
      if (!m_commands[i]->do_command()) {
	dbg0<<"Compound command member '"<<m_commands[i]->get_name()
	    <<"' failed!"<<std::endl;
	for (--i; i >= 0; --i)
	  m_commands[i]->undo_command();
	return false;
      }
    }
    return true;
  }


  bool CompoundCommand::undo_command() {
    m_used = true;
    for (int i = static_cast<int>(m_commands.size() - 1); i >= 0; --i) {
      if (!m_commands[i]->undo_command()) {
	for (++i; i < m_commands.size(); ++i)
	  m_commands[i]->do_command();
	return false;
      }
    }
    return true;
  }
  
  
  bool CompoundCommand::append(Command* cmd) {
    if (m_used)
      return false;
    for (unsigned i = 0; i < m_commands.size(); ++i) {
      if (m_commands[i] == cmd)
	return false;
    }
    m_commands.push_back(cmd);
  }
  

  void CompoundCommand::clear() {
    for (unsigned i = 0; i < m_commands.size(); ++i)
      delete m_commands[i];
    m_commands.clear();
    m_used = false;
  }


}
