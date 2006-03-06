#include "interpolatedevent.hpp"


namespace Dino {


  InterpolatedEvent::InterpolatedEvent(int param, int start, int end, 
				       unsigned step, unsigned length) 
    : m_param(param),
      m_start(start),
      m_end(end),
      m_step(step),
      m_length(length) {

  }
  
  
  int InterpolatedEvent::get_param() const {
    return m_param;
  }
  
  
  unsigned InterpolatedEvent::get_step() const {
    return m_step;
  }
  
  
  unsigned InterpolatedEvent::get_length() const {
    return m_length;
  }


  int InterpolatedEvent::get_start() const {
    return m_start;
  }
  
  
  int InterpolatedEvent::get_end() const {
    return m_end;
  }
  

  void InterpolatedEvent::set_step(unsigned int step) {
    m_step = step;
  }
  
  
  void InterpolatedEvent::set_length(unsigned int length) {
    m_length = length;
  }


  void InterpolatedEvent::set_start(int start) {
    m_start = start;
  }
  
  
  void InterpolatedEvent::set_end(int end) {
    m_end = end;
  }

  
}
