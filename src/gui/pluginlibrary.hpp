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

#ifndef PLUGINLIBRARY_HPP
#define PLUGINLIBRARY_HPP


#include <map>
#include <string>


class PluginInterface;
class Plugin;


class PluginLibrary {
public:
  
  class PluginInfo {
  public:
    PluginInfo(const std::string& _name = "", 
	       const std::string& _filename = "");
    std::string name;
    std::string filename;
  private:
    friend class PluginLibrary;
    void* module;
    bool loaded;
  };
  
  typedef std::map<std::string, PluginInfo>::const_iterator const_iterator;
  typedef std::map<std::string, PluginInfo>::iterator iterator;
  
  PluginLibrary(PluginInterface& plif);
  ~PluginLibrary();

  void refresh_list();
  
  bool is_loaded(const_iterator& iter) const;
  bool is_loaded(iterator& iter) const;
  
  void load_plugin(iterator& iter);
  
  void unload_plugin(iterator& iter);
  
  const_iterator begin() const;
  iterator begin();
  const_iterator end() const;
  iterator end();
  const_iterator find(const std::string& name) const;
  iterator find(const std::string& name);
  
  sigc::signal<void, iterator> signal_plugin_loaded;
  sigc::signal<void, iterator> signal_plugin_unloaded;
  
protected:
  
  /** Never use this unless you really know what you are doing! */
  template <class T, class S> T illegal_cast(S arg) {
    union {
      S v;
      T t;
    } vpf;
    vpf.v = arg;
    return vpf.t;
  }
  
  std::map<std::string, PluginInfo> m_plugins;
  PluginInterface& m_plif;
  
};


#endif
