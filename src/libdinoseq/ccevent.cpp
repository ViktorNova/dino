#include <cassert>

#include "ccevent.hpp"


namespace Dino {


  CCEvent::CCEvent(unsigned char param, unsigned char value, unsigned int step) 
    : MIDIEvent(MIDIEvent::Controller, param, value),
      m_earlier(NULL),
      m_later(NULL),
      m_step(step) {
    assert(param < 128);
    assert(value < 128);
  }


  unsigned char CCEvent::get_value() const {
    return m_data[2];
  }
  
  
  CCEvent* CCEvent::get_earlier() {
    return m_earlier;
  }
  
  
  CCEvent* CCEvent::get_later() {
    return m_later;
  }
  
  
  unsigned int CCEvent::get_step() {
    return m_step;
  }
  
  
  void CCEvent::set_value(unsigned char value) {
    m_data[2] = value;
  }


  void CCEvent::set_earlier(CCEvent* event) {
    m_earlier = event;
  }

  
  void CCEvent::set_later(CCEvent* event) {
    m_later = event;
  }
  
  
  void CCEvent::set_step(unsigned int step) {
    m_step = step;
  }
  

}
