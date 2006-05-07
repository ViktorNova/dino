#include "pluginlibrary.hpp"

PluginLibrary::PluginInfo::PluginInfo(const std::string& _name, 
				      const std::string& _filename)
  : name(_name),
    filename(_filename),
    module(0) {

}
 
 
void PluginLibrary::refresh_list() {
  
}

  
void PluginLibrary::load_plugin(const_iterator& iter) {
  
}
 
 
void PluginLibrary::unload_plugin(const_iterator& iter) {
  
}

  
