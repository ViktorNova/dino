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
#include <string>
#include <typeinfo>
#include <cxxabi.h>


namespace Dino {


#ifndef NDEBUG

  extern std::ostream& dbg0_real;
  extern std::ostream& dbg1_real;
  
  
  /* Macros are ugly, but in this case they are the only way to get the 
     __FILE__ and __LINE__ from the caller. */
#define dbg0 dbg0_real<<"\033[31;1m"<<'['<<std::setw(16)<<std::setfill(' ') \
    <<__FILE__<<':'<<std::setw(3)<<std::setfill('0')<<__LINE__<<"] "<<"\033[0m"
#define dbg1 dbg1_real<<"\033[32;1m"<<'['<<std::setw(16)<<std::setfill(' ') \
    <<__FILE__<<':'<<std::setw(3)<<std::setfill('0')<<__LINE__<<"] "<<"\033[0m"

#else


  /** If NDEBUG is defined @c dbg0 will simply be a reference to @c std::cerr. */
  extern ostream& dbg0;

  struct NoOpStream {

  };

  extern NoOpStream dbg1;


  /** This function will just return a reference to the parameter @c a. It is
      used to turn off debug printing when @c NDEBUG is defined. */
  template <class T>
  inline NoOpStream& operator<<(NoOpStream& a, const T&) { 
    return a;
  }

  /** This function will just return a reference to the parameter @c a. It is
      used to turn off debug printing when @c NDEBUG is defined. */
  inline NoOpStream& operator<<(NoOpStream& a, ostream& (*__pf)(ostream&)) { 
    return a;
  }

#endif


  /** This function is probably GCC specific - I'll try to fix it if I ever
      need to build it on another compiler. */
  inline std::string demangle(const std::string& str) {
    static int status;
    return abi::__cxa_demangle(str.c_str(), 0, 0, &status);
  }


}


#endif
