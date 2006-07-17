#include <cassert>
#include <iostream>

#include <rasqal.h>

#include <slv2/query.h>

#include "lv2host.hpp"


using namespace std;


LV2Host::LV2Host(const string& uri, unsigned long frame_rate) 
  : m_instance(0),
    m_handle(0),
    m_inst_desc(0),
    m_midimap(128, -1) {
  
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
    
    // set the instrument function callback using nasty union cast
    LV2_Host_Feature instrument_feature;
    instrument_feature.URI = "<http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext>";
    union {
      LV2_InstrumentFunctionCallback lifc;
      void* v;
    } u;
    u.lifc = &LV2Host::instrument_descriptor_callback;
    instrument_feature.data = u.v;
    const LV2_Host_Feature* features[2];
    features[0] = &instrument_feature;
    features[1] = 0;
    
    // nasty workaround for C callback
    m_current_object = this;
    m_instance = slv2_plugin_instantiate(plugin, frame_rate, features);
    m_current_object = 0;
    
    if (m_instance) {
      for (size_t i = 0; i < slv2_plugin_get_num_ports(plugin); ++i) {
        bool midi = false;
        SLV2DataType type = slv2_port_get_data_type(plugin, i);
        SLV2PortClass pclass = slv2_port_get_class(plugin, i);
        
        // check the datatype
        if (type != SLV2_DATA_TYPE_FLOAT) {
          SLV2Property prop = 
            slv2_port_get_property(plugin, i, (unsigned char*)("lv2:datatype"));
          cerr<<"port "<<i<<" has type "<<prop->values[0]<<endl;
          if (string("http://ll-plugins.nongnu.org/lv2/namespace#miditype") !=
              (const char*)prop->values[0]) {
            slv2_instance_free(m_instance);
            m_instance = 0;
            for (int j = 0; j < prop->num_values; ++j)
              free(prop->values[j]);
            free(prop);
            // XXX leak! no slv2_plugin_free(m_instance)
            return;
          }
          midi = true;
          for (int j = 0; j < prop->num_values; ++j)
            free(prop->values[j]);
          free(prop);
        }
        
        LV2Port p;
        p.buffer = 0;
        p.port_class = slv2_port_get_class(plugin, i);
        char* symbol = (char*)slv2_port_get_symbol(plugin, i);
        p.symbol = symbol;
        p.default_value = slv2_port_get_default_value(plugin, i);
        p.midi = midi;
        free(symbol);
        m_ports.push_back(p);
      }
      m_handle = slv2_instance_get_handle(m_instance);
      
      // get the default MIDI controllers
      char q[] = 
        "SELECT DISTINCT ?index, ?controller FROM data: WHERE {\n"
        "plugin: lv2:port ?port\n"
        "?port lv2:index ?index\n"
        "?port <http://ll-plugins.nongnu.org/lv2/namespace#defaultMidiController> ?cont\n"
        "?cont <http://ll-plugins.nongnu.org/lv2/namespace#controllerType> <http://ll-plugins.nongnu.org/lv2/namespace#CC>\n"
        "?cont <http://ll-plugins.nongnu.org/lv2/namespace#controllerNumber> ?controller.\n}\n";
      
      cerr<<1<<endl;
      
      rasqal_query_results* results = 
        slv2_plugin_run_query(plugin, (unsigned char*)q, 0);
      cerr<<2<<endl;
      
      while(!rasqal_query_results_finished(results)) {

        cerr<<3<<endl;
        for(int i = 0; 
            i < rasqal_query_results_get_bindings_count(results); i++) {

          cerr<<4<<endl;

          const unsigned char* name = rasqal_query_results_get_binding_name(results,i);
          rasqal_literal* value = rasqal_query_results_get_binding_value(results,i);
          cerr<<(const char*)name<<" -> "<<(const char*)value<<endl;
        }
        cerr<<endl;
        rasqal_query_results_next(results);
      }
      
      /*
      if (type == SLV2_DATA_TYPE_FLOAT && pclass == SLV2_CONTROL_RATE_INPUT) {
        char dmc[] = 
          "<http://ll-plugins.nongnu.org/lv2/namespace#defaultMidiController>";
        SLV2Property prop = 
          slv2_port_get_property(plugin, i, (unsigned char*)dmc);
        if (prop->num_values > 0) {
          
          cerr<<prop->num_values<<" "<<((char*)prop->values[0])<<endl;
          m_midimap[atoi((char*)prop->values[0])] = i;
        }
        for (int j = 0; j < prop->num_values; ++j)
          free(prop->values[j]);
        free(prop);
      }
      */
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


char* LV2Host::configure(const char* key, const char* value) {
  if (m_inst_desc && m_inst_desc->configure)
    return m_inst_desc->configure(m_handle, key, value);
  else
    cerr<<"This plugin has no configure() callback"<<endl;
  return 0;
}


unsigned int LV2Host::list_used_files(char*** keys, char*** filepaths) {
  if (m_inst_desc && m_inst_desc->list_used_files)
    return m_inst_desc->list_used_files(m_handle, keys, filepaths);
  else
    cerr<<"This plugin has no list_used_files() callback"<<endl;
  return 0;
}


const LV2_ProgramDescriptor* LV2Host::get_program(unsigned long index) {
  if (m_inst_desc && m_inst_desc->get_program)
    return m_inst_desc->get_program(m_handle, index);
  else
    cerr<<"This plugin has no get_program() callback"<<endl;
  return 0;
}


void LV2Host::select_program(unsigned long program) {
  if (m_inst_desc && m_inst_desc->select_program)
    m_inst_desc->select_program(m_handle, program);
}


void LV2Host::instrument_descriptor_callback(LV2_InstrumentFunction func){
  m_current_object->m_inst_desc = func();
}


const std::vector<int>& LV2Host::get_midi_map() const {
  return m_midimap;
}


LV2Host* LV2Host::m_current_object(0);
