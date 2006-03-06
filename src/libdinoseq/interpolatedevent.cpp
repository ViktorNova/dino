#include "interpolatedevent.hpp"


namespace Dino {


  InterpolatedEvent::InterpolatedEvent(int param, int value, int step) 
    : m_param(param),
      m_start(value),
      m_step(step) {

  }
  
  
  int InterpolatedEvent::get_param() const {
    return m_param;
  }
  
  
  int InterpolatedEvent::get_step() const {
    return m_step;
  }
  
  
  int InterpolatedEvent::get_length() const {
    return m_length;
  }


  int InterpolatedEvent::get_start() const {
    return m_start;
  }
  
  
  int InterpolatedEvent::get_end() const {
    return m_end;
  }
  
  
}
