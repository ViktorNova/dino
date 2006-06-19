#ifndef LV2HOST_HPP
#define LV2HOST_HPP

#include <string>
#include <vector>

#include <slv2/slv2.h>


struct LV2Port {
  void* buffer;
  std::string symbol;
  SLV2PortClass port_class;
  float default_value;
};


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
  
protected:
  
  SLV2Instance* m_instance;
  std::vector<LV2Port> m_ports;
};


#endif
