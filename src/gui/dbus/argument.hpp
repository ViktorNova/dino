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

#ifndef ARGUMENT_HPP
#define ARGUMENT_HPP

#include <string>


namespace DBus {

  
  /** This class wraps an argument to a D-Bus method handler.
      @seeAlso DBus::Object::add_method */
  class Argument {
  public:
    
    /** This default constructor creates an Argument object with the type 
	INVALID. */
    Argument();
    /** Creates an integer argument. */
    Argument(int value);
    /** Creates a double precision floating point argument. */
    Argument(double value);
    /** Creates a string argument. */
    Argument(const char* value, bool is_error = false);
    
    /** The different types that an argument can have. */
    enum Type {
      INT,
      DOUBLE,
      STRING,
      ERROR,
      INVALID
    } type;
    
    
    union {
      int i;
      double d;
      const char* s;
      const char* e;
    };
    
  };


}


#endif
