#ifndef MIDIBUFFER_HPP
#define MIDIBUFFER_HPP


namespace Dino {
  
  
  class MIDIBuffer {
  public:
    
    MIDIBuffer(void* port_buffer);
    
    void set_period_size(unsigned long nframes);
    
    void set_cc_resolution(double beats);
    
    double get_cc_resolution() const;
    
    unsigned char* reserve(double beat, size_t data_size);
    
    int write(double beat, const unsigned char* data, size_t data_size); 
    
  protected:
    
    void* m_buffer;
    unsigned long m_nframes;
    double m_cc_resolution;
  };


}


#endif
