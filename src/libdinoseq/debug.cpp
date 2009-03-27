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

#include "debug.hpp"


using namespace std;


namespace Dino {

  ConcatenatorFactory cc;

  sigc::signal<void, int, const std::string&, 
	       int, const std::string&> signal_debug;

#ifndef NDEBUG 

  void dbg_real(unsigned level, const std::string& file, 
		unsigned line, const std::string& msg) {
    signal_debug(level, file, line, msg);
    cerr<<(level ? "\033[32;1m" : "\033[31;1m")
	<<'['<<std::setw(16)<<std::setfill(' ')<<file<<':'
	<<setw(3)<<std::setfill('0')<<__LINE__<<"] "
	<<"\033[0m"<<msg<<endl;
  }
  
#endif


}
