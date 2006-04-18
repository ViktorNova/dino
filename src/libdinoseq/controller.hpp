#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <string>
#include <vector>

#include "interpolatedevent.hpp"


namespace Dino {


  class Controller {
  public:
    
    Controller(const std::string& name, unsigned int size, 
	       long param, int min, int max);
    ~Controller();
    
    const std::string& get_name() const;
    const InterpolatedEvent* get_event(unsigned int step) const;
    int get_min() const;
    int get_max() const;
    long get_param() const;
    unsigned int get_size() const;
    
    void set_name(const std::string& name);
    void add_point(unsigned int step, int value);
    void remove_point(unsigned int step);

  private:
    
    std::string m_name;
    std::vector<InterpolatedEvent*> m_events;
    long m_param;
    int m_min;
    int m_max;
    
  };

  
}


#endif
