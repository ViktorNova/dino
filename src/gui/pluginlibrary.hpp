#ifndef PLUGINLIBRARY_HPP
#define PLUGINLIBRARY_HPP


#include <map>
#include <string>


namespace Glib {
  class Module;
}

class PluginInterface;


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
    Glib::Module* module;
  };
  
  typedef std::map<std::string, PluginInfo>::const_iterator const_iterator;
  typedef std::map<std::string, PluginInfo>::iterator iterator;
  
  PluginLibrary(PluginInterface& plif);
  ~PluginLibrary();

  void refresh_list();
  
  void load_plugin(iterator& iter);
  
  void unload_plugin(iterator& iter);
  
  const_iterator begin() const;
  iterator begin();
  const_iterator end() const;
  iterator end();
  const_iterator find(const std::string& name) const;
  iterator find(const std::string& name);
  
protected:
  
  std::map<std::string, PluginInfo> m_plugins;
  PluginInterface& m_plif;
  
};


#endif
