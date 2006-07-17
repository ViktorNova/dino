#ifndef LV2HOST_HPP
#define LV2HOST_HPP

#include <string>
#include <vector>

#include <slv2/slv2.h>

#include "lv2-instrument.h"


/** A struct that holds information about a port in a LV2 plugin. */
struct LV2Port {
  void* buffer;
  std::string symbol;
  SLV2PortClass port_class;
  float default_value;
  bool midi;
};


/** A class that loads a single LV2 plugin. */
class LV2Host {
public:
  LV2Host(const std::string& uri, unsigned long frame_rate);
  ~LV2Host();
  
  bool is_valid() const;
  
  const std::vector<LV2Port>& get_ports() const;
  std::vector<LV2Port>& get_ports();
  
  void activate();
  void run(unsigned long nframes);
  void deactivate();
  
  char* configure(const char* key, const char* value);
  unsigned int list_used_files(char*** keys, char*** filepaths);
  const LV2_ProgramDescriptor* get_program(unsigned long index);
  void select_program(unsigned long program);
  
  const std::vector<int>& get_midi_map() const;
  
protected:
  
  static void instrument_descriptor_callback(LV2_InstrumentFunction func);
  
  static LV2Host* m_current_object;
  
  SLV2Instance* m_instance;
  LV2_Handle m_handle;
  const LV2_InstrumentDescriptor* m_inst_desc;
  
  std::vector<LV2Port> m_ports;
  std::vector<int> m_midimap;
};


#endif
