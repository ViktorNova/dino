#ifndef CCEVENT_HPP
#define CCEVENT_HPP

#include "midievent.hpp"

namespace Dino {

  
  class CCEvent : public MIDIEvent {
  public:
    
    CCEvent(unsigned char param, unsigned char value, unsigned int step);
    
    /** Return the third data byte of this event. For a controller event this
	will be the controller value. */
    unsigned char get_value() const;
    CCEvent* get_earlier();
    CCEvent* get_later();
    unsigned int get_step();
    
    /** Set the third byte of the event. For controller events this is the
	controller value. */
    void set_value(unsigned char value);
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
