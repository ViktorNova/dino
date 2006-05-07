#ifndef PLUGINLIBRARY_HPP
#define PLUGINLIBRARY_HPP


#include <map>
#include <string>


namespace Glib {
  class Module;
}


class PluginLibrary {
public:
  
  class PluginInfo {
  public:
    PluginInfo(const std::string& _name, const std::string& _filename);
    std::string name;
    std::string filename;
  private:
    friend class PluginLibrary;
    Glib::Module* module;
  };
  
  typedef std::map<std::string, PluginInfo>::const_iterator const_iterator;
  typedef std::map<std::string, PluginInfo>::iterator iterator;
  
  void refresh_list();
  
  void load_plugin(const_iterator& iter);
  
  void unload_plugin(const_iterator& iter);
  
protected:
  
  std::map<std::string, PluginInfo> m_plugins;
  
};


#endif
