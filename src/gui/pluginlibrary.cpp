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
	  dbg0<<"Shared module \""<<(*iter)
	      <<"\" has no dino_plugin() callback"<<endl;
	}
	dlclose(mod);
      }
      else
	dbg0<<"Could not load module \""<<(*iter)<<"\": "<<dlerror()<<endl;
    }
  }
  catch (...) {
    dbg0<<"Could not open plugin directory"<<endl;
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
    dbg1<<"Loading plugin \""<<iter->second.name<<endl;
    if (!iter->second.module) {
      string filename = string(PLUGIN_DIR) + "/" + iter->second.filename;
      iter->second.module = dlopen(filename.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    }
    if (!iter->second.module)
      dbg0<<"Could not load module \""<<iter->second.filename<<"\""<<endl;
    else {
      void* plug;
      if (!(plug = dlsym(iter->second.module, "dino_load_plugin"))) {
	dlclose(iter->second.module);
	iter->second.module = 0;
	dbg0<<"Could not load plugin from file \""
	    <<iter->second.filename<<"\""<<endl;
      }
      else {
	(*illegal_cast<PluginLoadFunc>(plug))(m_plif);
	iter->second.loaded = true;
	signal_plugin_loaded(iter);
      }
    }
  }
}
 
 
void PluginLibrary::unload_plugin(iterator& iter) {
  if (iter->second.loaded) {
    dbg1<<"Unloading plugin \""<<iter->second.name<<"\""<<endl;
    void* plug;
    if (!(plug = dlsym(iter->second.module, "dino_unload_plugin"))) {
      dbg0<<"Could not unload plugin \""<<iter->second.name
	  <<"\" - it has no unload function!"<<endl;
    }
    else {
      (*illegal_cast<PluginUnloadFunc>(plug))();
      iter->second.loaded = false;
      signal_plugin_unloaded(iter);
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
