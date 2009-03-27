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

#include <dlfcn.h>

#include "debug.hpp"
#include "plugininterface.hpp"
#include "pluginlibrary.hpp"


using namespace Glib;
using namespace std;
using namespace Dino;


PluginLibrary::PluginInfo::PluginInfo(const std::string& _name, 
                                      const std::string& _filename)
  : name(_name),
    filename(_filename),
    module(0),
    loaded(false) {
  
}


PluginLibrary::PluginLibrary(PluginInterface& plif) 
  : m_plif(plif) {
  
  try {
    Dir plugin_dir(PLUGIN_DIR);
    Dir::const_iterator iter;
    for (iter = plugin_dir.begin(); iter != plugin_dir.end(); ++iter) {
      string filename = string(PLUGIN_DIR) + "/" + *iter;
      void* mod = dlopen(filename.c_str(), RTLD_LAZY | RTLD_GLOBAL);
      if (mod) {
        void* plug;
        if ((plug = dlsym(mod, "dino_get_name"))) {
          string name = (*illegal_cast<PluginNameFunc>(plug))();
          m_plugins[name] = PluginInfo(name, *iter);
        }
        else {
          dbg(0, cc+ "Shared module \"" + (*iter) + 
              "\" has no dino_plugin() callback");
        }
        dlclose(mod);
      }
      else
        dbg(0, cc+ "Could not load module \"" + (*iter) + 
	    "\": " + dlerror());
    }
  }
  catch (...) {
    dbg(0, "Could not open plugin directory");
    m_plugins.clear();
  }
}


PluginLibrary::~PluginLibrary() {
  for (iterator iter = begin(); iter != end(); ++iter)
    unload_plugin(iter);
}


void PluginLibrary::refresh_list() {

}

  
void PluginLibrary::load_plugin(iterator& iter) {
  if (!iter->second.loaded) {
    dbg(1, cc+ "Loading plugin \"" + iter->second.name + '"');
    if (!iter->second.module) {
      string filename = string(PLUGIN_DIR) + "/" + iter->second.filename;
      iter->second.module = dlopen(filename.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    }
    if (!iter->second.module) {
      dbg(0, cc+ "Could not load module \"" + iter->second.filename + "\"");
      m_plif.set_status(string("Could not load module ") +
                        iter->second.filename);
    }
    else {
      void* plug;
      if (!(plug = dlsym(iter->second.module, "dino_load_plugin"))) {
        dlclose(iter->second.module);
        iter->second.module = 0;
        dbg(0, cc+ "Could not load plugin from file \"" +
            iter->second.filename + "\"");
        m_plif.set_status(string("Could not load plugin from file ") + 
                          iter->second.filename);
      }
      else {
        (*illegal_cast<PluginLoadFunc>(plug))(m_plif);
        iter->second.loaded = true;
        signal_plugin_loaded(iter);
        m_plif.set_status(string("Loaded ") + iter->second.name);
      }
    }
  }
}
 
 
void PluginLibrary::unload_plugin(iterator& iter) {
  if (iter->second.loaded) {
    dbg(1, cc+ "Unloading plugin \"" + iter->second.name + "\"");
    void* plug;
    if (!(plug = dlsym(iter->second.module, "dino_unload_plugin"))) {
      dbg(0, cc+ "Could not unload plugin \"" + iter->second.name +
          "\" - it has no unload function!");
      m_plif.set_status(string("Could not unload ") + iter->second.name + "!");
    }
    else {
      (*illegal_cast<PluginUnloadFunc>(plug))();
      iter->second.loaded = false;
      signal_plugin_unloaded(iter);
      m_plif.set_status(string("Unloaded ") + iter->second.name);
    }
  }
}

  
PluginLibrary::const_iterator PluginLibrary::begin() const {
  return m_plugins.begin();
}


PluginLibrary::iterator PluginLibrary::begin() {
  return m_plugins.begin();
}


PluginLibrary::const_iterator PluginLibrary::end() const {
  return m_plugins.end();
}


PluginLibrary::iterator PluginLibrary::end() {
  return m_plugins.end();
}


PluginLibrary::const_iterator PluginLibrary::find(const std::string& name) const {
  return m_plugins.find(name);
}


PluginLibrary::iterator PluginLibrary::find(const std::string& name) {
  return m_plugins.find(name);
}


bool PluginLibrary::is_loaded(const_iterator& iter) const {
  return iter->second.loaded;
}


bool PluginLibrary::is_loaded(iterator& iter) const {
  return iter->second.loaded;
}
