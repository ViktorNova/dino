#include <iostream>

#include "plugininterface.hpp"


using namespace std;


extern "C" {
  
  string dino_get_name() {
    cerr<<"TestPlugin::"<<__FUNCTION__<<endl;
    return "Test plugin";
  }
  
  
  void dino_load_plugin(PluginInterface& plif) {
    cerr<<"TestPlugin::"<<__FUNCTION__<<endl;
  }
  
  
  void dino_unload_plugin() {
    cerr<<"TestPlugin::"<<__FUNCTION__<<endl;    
  }
}
