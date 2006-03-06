#ifndef INTERPOLATEDEVENT_HPP
#define INTERPOLATEDEVENT_HPP


namespace Dino {
  
  
  class InterpolatedEvent {
  public:
    
    InterpolatedEvent(int param, int value, int step);
    
    int get_param() const;
    int get_step() const;
    int get_length() const;
    int get_start() const;
    int get_end() const;
    
  protected:
    
    int m_param;
    int m_start;
    int m_end;
    int m_step;
    int m_length;
    
  };


}


#endif
