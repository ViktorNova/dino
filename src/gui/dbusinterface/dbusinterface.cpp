/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, 
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include <cstdlib>
#include <iostream>

#include "note.hpp"
#include "pattern.hpp"
#include "patternselection.hpp"
#include "plugininterface.hpp"
#include "sequencer.hpp"
#include "song.hpp"
#include "track.hpp"


using namespace Dino;
using namespace std;


namespace {
  PluginInterface* m_plif = 0;
}


extern "C" {
  string dino_get_name() { 
    return "DBUS interface"; 
  }
  
  void dino_load_plugin(PluginInterface& plif) {
    m_plif = &plif;
  }
  
  void dino_unload_plugin() {

  }
}


