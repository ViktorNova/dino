#ifndef INTERPOLATEDEVENT_HPP
#define INTERPOLATEDEVENT_HPP


namespace Dino {
  
  
  class InterpolatedEvent {
  public:
    
    InterpolatedEvent(int param, int start, int end, 
		      unsigned step, unsigned length);
    
    int get_param() const;
    unsigned get_step() const;
    unsigned get_length() const;
    int get_start() const;
    int get_end() const;

    void set_step(unsigned step);
    void set_length(unsigned length);
    void set_start(int start);
    void set_end(int end);
    
  protected:
    
    int m_param;
    int m_start;
    int m_end;
    unsigned m_step;
    unsigned m_length;
    
  };


}


#endif
