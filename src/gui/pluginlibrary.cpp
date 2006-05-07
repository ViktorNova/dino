#include <glibmm.h>
#include <glibmm/module.h>

#include "debug.hpp"
#include "plugininterface.hpp"
#include "pluginlibrary.hpp"


using namespace Glib;
using namespace std;


PluginLibrary::PluginInfo::PluginInfo(const std::string& _name, 
				      const std::string& _filename)
  : name(_name),
    filename(_filename),
    module(0) {

}


PluginLibrary::PluginLibrary(PluginInterface& plif) 
  : m_plif(plif) {
  
  Dir plugin_dir(PLUGIN_DIR);
  Dir::const_iterator iter;
  for (iter = plugin_dir.begin(); iter != plugin_dir.end(); ++iter) {
    string filename = string(PLUGIN_DIR) + "/" + *iter;
    Module mod(filename, ModuleFlags(3));
    if (mod) {
      void* plug;
      if (mod.get_symbol("dino_plugin", plug)) {
	string name = ((Plugin*)plug)->get_name();
	m_plugins[name] = PluginInfo(name, filename);
      }
    }
  }
}


PluginLibrary::~PluginLibrary() {
  for (iterator iter = begin(); iter != end(); ++iter)
    unload_plugin(iter);
}


void PluginLibrary::refresh_list() {

}

  
void PluginLibrary::load_plugin(iterator& iter) {
  if (!iter->second.module) {
    iter->second.module = new Module(iter->second.filename, ModuleFlags(3));
    if (!*iter->second.module) {
      iter->second.module = 0;
      dbg0<<"Could not load module \""<<iter->second.filename<<"\""<<endl;
    }
    else {
      void* plug;
      if (!iter->second.module->get_symbol("dino_plugin", plug)) {
	delete iter->second.module;
	iter->second.module = 0;
	dbg0<<"Could not load plugin from file \""
	    <<iter->second.filename<<"\""<<endl;
      }
      else
	static_cast<Plugin*>(plug)->initialise(m_plif);
    }
  }
}
 
 
void PluginLibrary::unload_plugin(iterator& iter) {
  delete iter->second.module;
  iter->second.module = 0;
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


