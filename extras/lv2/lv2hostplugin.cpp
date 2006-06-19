#include <iostream>

#include <debug.hpp>
#include <plugininterface.hpp>
#include <lo/lo.h>

#include "lv2widget.hpp"


using namespace std;
using namespace Dino;
using namespace Glib;
using namespace sigc;


// variables and functions local to the plugin
namespace {
  class LV2HostUI : public GUIPage {
  public: 
    LV2HostUI() {
      pack_start(*manage(new LV2Widget("http://plugin.org.uk/swh-plugins/amp")));
    }
  };
  
  PluginInterface* m_plif = 0;
  LV2HostUI* m_ui = 0;
}


// required plugin interface
extern "C" {
  
  string dino_get_name() {
    return "LV2 host";
  }
  
  void dino_load_plugin(PluginInterface& plif) {
    m_ui = manage(new LV2HostUI);
    plif.add_page("LV2", *m_ui);
    m_plif = &plif;
  }
  
  void dino_unload_plugin() {
    m_plif->remove_page(*m_ui);
  }

}


