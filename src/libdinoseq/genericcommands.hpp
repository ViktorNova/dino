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

#ifndef GENERICCOMMANDS_HPP
#define GENERICCOMMANDS_HPP

#include <string>

#include "command.hpp"


namespace Dino {
  
  
  /** This template class can be used as a command that modifies a string
      property of an object with get and set methods, for example to 
      change the title of a Song object using Song::get_title() and 
      Song::set_title(). */
  template <class C>
  class SetString : public Command {
  public:
    
    /** Create a new undoable command that sets a string property of @c object
	to @c new_prop. @c object must not be destroyed before this Command
	instance, since it is used in the do_command() and undo_command() 
	functions. */
    SetString(const std::string& name, 
	      C& object, 
	      const std::string& new_prop,
	      const std::string& (C::*getter)() const, 
	      void (C::*setter)(const std::string&))
      : Command(name),
	m_object(object),
	m_new(new_prop), 
	m_getter(getter),
	m_setter(setter) {

    }
    
    /** Change the property to the new value and store the old value. */
    bool do_command() {
      m_old = (m_object.*m_getter)();
      (m_object.*m_setter)(m_new);
      return true;
    }
    
    /** Restore the property to the old value. */
    bool undo_command() {
      (m_object.*m_setter)(m_old);
      return true;
    }
    
  protected:
    
    C& m_object;
    std::string m_old;
    std::string m_new;
    const std::string& (C::*m_getter)() const;
    void (C::*m_setter)(const std::string&);
    
  };
  

}


#endif
