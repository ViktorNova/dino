#include <cassert>

#include "lv2host.hpp"


using namespace std;


LV2Host::LV2Host(const string& uri, unsigned long frame_rate) 
  : m_instance(0) {
  
  // find the plugin
  SLV2List plugins = slv2_list_new();
	slv2_list_load_all(plugins);
  SLV2Plugin* plugin = slv2_list_get_plugin_by_uri(plugins, (unsigned char*)
                                                   uri.c_str());
  if (plugin)
    plugin = slv2_plugin_duplicate(plugin);
  slv2_list_free(plugins);
  
  // instantiate it and get info
  if (plugin) {
    const LV2_Host_Feature* features[1];
    features[0] = 0;
    m_instance = slv2_plugin_instantiate(plugin, frame_rate, features);
    if (m_instance) {
      for (size_t i = 0; i < slv2_plugin_get_num_ports(plugin); ++i) {
        SLV2DataType type = slv2_port_get_data_type(plugin, i);
        if (type != SLV2_DATA_TYPE_FLOAT) {
          slv2_instance_free(m_instance);
          m_instance = 0;
          // XXX leak! slv2_plugin_free(m_instance);
          return;
        }
        LV2Port p;
        p.buffer = 0;
        p.port_class = slv2_port_get_class(plugin, i);
        char* symbol = (char*)slv2_port_get_symbol(plugin, i);
        p.symbol = symbol;
        p.default_value = slv2_port_get_default_value(plugin, i);
        free(symbol);
        m_ports.push_back(p);
      }
    }
  }
}


LV2Host::~LV2Host() {
  if (m_instance)
    slv2_instance_free(m_instance);
}


bool LV2Host::is_valid() const {
  return (m_instance != 0);
}


const vector<LV2Port>& LV2Host::get_ports() const {
  assert(m_instance);
  return m_ports;
}


vector<LV2Port>& LV2Host::get_ports() {
  assert(m_instance);
  return m_ports;
}


void LV2Host::activate() {
  assert(m_instance);
  slv2_instance_activate(m_instance);
}


void LV2Host::run(unsigned long nframes) {
  assert(m_instance);
  for (size_t i = 0; i < m_ports.size(); ++i)
    slv2_instance_connect_port(m_instance, i, m_ports[i].buffer);
  slv2_instance_run(m_instance, nframes);
}


void LV2Host::deactivate() {
  assert(m_instance);
  slv2_instance_deactivate(m_instance);
}

