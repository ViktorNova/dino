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

#include "connection.hpp"
#include "dinoobject.hpp"
#include "plugininterface.hpp"


using namespace Dino;
using namespace std;
using namespace sigc;


namespace DBus {
  class Argument;
}


namespace {
  PluginInterface* plif = 0;
  DBus::Connection* dbus = 0;
  connection idle;
}


extern "C" {
  string dino_get_name() { 
    return "DBUS interface"; 
  }
  
  void dino_load_plugin(PluginInterface& p) {
    plif = &p; 
    dbus = new DBus::Connection("org.nongnu.dino");
    DBus::Object* obj = new DinoObject(plif->get_command_proxy());
    //obj->add_method("org.nongnu.dino.Sequencer", "Play", "", 
    //		    sigc::ptr_fun(foo));
    //obj->add_method("org.nongnu.dino.Sequencer", "Stop", "", 
    //		    sigc::ptr_fun(foo));
    //obj->add_method("org.nongnu.dino.Sequencer", "GoToBeat", "", 
    //		    sigc::ptr_fun(foo));
    dbus->register_object("/", obj);
    idle = Glib::signal_timeout().
      connect(bind(mem_fun(*dbus, &DBus::Connection::run), 0), 10);
  }
  
  void dino_unload_plugin() {
    if (dbus) {
      idle.disconnect();
      delete dbus;
    }
  }
}

