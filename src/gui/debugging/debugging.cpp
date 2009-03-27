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

#include <cstdlib>
#include <iostream>

#include "debuggingpage.hpp"
#include "plugininterface.hpp"


using namespace Dino;
using namespace std;


namespace {
  PluginInterface* g_plif = 0;
  DebuggingPage* g_dbp = 0;
}


extern "C" {
  
  string dino_get_name() { 
    return "Debugging"; 
  }
  
  void dino_load_plugin(PluginInterface& plif) {
    g_plif = &plif;
    cout<<"Loaded the debugging plugin!"<<endl;
    g_dbp = manage(new DebuggingPage(g_plif->get_command_proxy()));
    g_plif->add_page("Debugging", *g_dbp);
  }
  
  void dino_unload_plugin() {
    cout<<"Unloaded the debugging plugin!"<<endl;
    g_plif->remove_page(*g_dbp);
  }
  
}


