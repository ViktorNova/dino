#ifndef MIDIBUFFER_HPP
#define MIDIBUFFER_HPP


namespace Dino {
  
  
  class MIDIBuffer {
  public:
    
    MIDIBuffer(void* port_buffer);
    
    void set_period_size(unsigned long nframes);
    
    unsigned char* reserve(double beat, size_t data_size);
    
    int write(double beat, const unsigned char* data, size_t data_size); 
    
  protected:
    
    void* m_buffer;
    unsigned long m_nframes;
    
  };


}


#endif
