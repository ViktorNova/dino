#include <iostream>

#include <debug.hpp>
#include <slv2/slv2.h>

#include "lv2widget.hpp"


using namespace Gtk;
using namespace std;
using namespace sigc;


LV2Widget::LV2Widget(const std::string& uri) {
  
  SLV2List plugins = slv2_list_new();
	slv2_list_load_all(plugins);
  SLV2Plugin* plugin = slv2_list_get_plugin_by_uri(plugins, (unsigned char*)
                                                   (unsigned char*)uri.c_str());
  if (!plugin) {
    dbg0<<"Could not find LV2 plugin <"<<uri<<">"<<endl;
    return;
  }
  
  system((string("dino-lv2host ") + uri + " &").c_str());
  m_address = lo_address_new_from_url("osc.udp://localhost:23483");
  
  int controls = 0;
  for (int i = 0; i < slv2_plugin_get_num_ports(plugin); ++i) {
    if (slv2_port_get_data_type(plugin, i) == SLV2_DATA_TYPE_FLOAT &&
        slv2_port_get_class(plugin, i) == SLV2_CONTROL_RATE_INPUT) {
      ++controls;
      resize(controls, 2);
      char* symbol = (char*)slv2_port_get_symbol(plugin, i);
      string sym(symbol);
      free(symbol);
      attach(*manage(new Label(sym)), 0, 1, controls - 1, controls, FILL);
      float min = slv2_port_get_minimum_value(plugin, i);
      float max = slv2_port_get_maximum_value(plugin, i);
      float def = slv2_port_get_default_value(plugin, i);
      HScale* scale = manage(new HScale(min, max, 0.001));
      scale->set_value(def);
      scale->signal_value_changed().
        connect(compose(bind<0>(mem_fun(*this, &LV2Widget::send_control), i),
                        mem_fun(*scale, &Scale::get_value)));
      attach(*scale, 1, 2, controls - 1, controls);
    }
  }
  
  slv2_list_free(plugins);
}

  
void LV2Widget::send_control(int port, float value) {
  cerr<<"change "<<port<<" to "<<value<<endl;
  lo_send(m_address, "/control", "if", port, value);
}

