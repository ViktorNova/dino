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

#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <typeinfo>

#include <cxxabi.h>

#include <sigc++/signal.h>
#include <glibmm/ustring.h>


namespace Dino {
  
  
  /** A class that helps concatenate different objects into strings,
      as long as they have a stream output operator. */
  class Concatenator {
  public:
    
    /** Default constructor does nothing. */
    Concatenator() {

    }
    
    /** This is needed because ostringstreams can't be copied. */
    Concatenator(const Concatenator& cnc) {
      m_oss.str(cnc.m_oss.str());
    }
    
    /** Add @c obj to the concatenated string. */
    template <typename T>
    Concatenator& operator+(const T& obj) {
      m_oss<<obj;
      return *this;
    }
    
    /** Cast operator to any type that can be automatically casted from a
	std::string, for convenience. */
    template <typename T>
    operator T() {
      return m_oss.str();
    }
    
    /** Cast operator to const char*. */
    operator const char*() {
      return m_oss.str().c_str();
    }

    
    std::ostringstream m_oss;

  };
  
  
  /** A class that creates a Concatenator object. */
  class ConcatenatorFactory {
  public:
    
    template <typename T>
    Concatenator operator+(const T& obj) {
      Concatenator cnc;
      cnc + obj;
      return cnc;
    }

  };
  
  
  extern ConcatenatorFactory cc;
  
  extern sigc::signal<void, int, const std::string&, 
		      int, const std::string&> signal_debug;
  
  
#ifndef NDEBUG

  void dbg_real(unsigned level, const std::string& file, 
		unsigned line, const std::string& msg);

  /* Macros are ugly, but in this case they are the only way to get the 
     __FILE__ and __LINE__ from the caller. */
#define dbg(LEVEL, MSG) dbg_real(LEVEL, __FILE__, __LINE__, MSG)

#else

  /** If NDEBUG is defined @c dbg will simply print to std::cerr if the level
      is 0. */
#define dbg(LEVEL, MSG) do { if (!LEVEL) std::cerr<<MSG<<std::endl; } while (false)

#endif


  /** This function is probably GCC specific - I'll try to fix it if I ever
      need to build it on another compiler. */
  inline std::string demangle(const std::string& str) {
    static int status;
    return abi::__cxa_demangle(str.c_str(), 0, 0, &status);
  }


}


#endif
