#ifndef CCEVENT_HPP
#define CCEVENT_HPP

#include "midievent.hpp"

namespace Dino {

  
  class CCEvent : public MIDIEvent {
  public:
    
    CCEvent(unsigned char param, unsigned char value, unsigned int step);
    
    CCEvent* get_earlier();
    CCEvent* get_later();
    unsigned int get_step();
    
    void set_earlier(CCEvent* event);
    void set_later(CCEvent* event);
    void set_step(unsigned int step);
    
  protected:
    
    CCEvent* m_earlier;
    CCEvent* m_later;
    unsigned int m_step;
    
  };


}


#endif
