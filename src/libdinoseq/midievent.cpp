#include <iostream>

#include "midievent.hpp"


using namespace std;


namespace Dino {


  MIDIEvent MIDIEvent::AllNotesOff(0xB0, 123, 0);
  MIDIEvent MIDIEvent::ChannelVolume(0xB0, 7, 10);

  
  unsigned char MIDIEvent::get_type() const {
    const unsigned char& sb = m_data[0];
    if (sb >= 0x80 && sb < 0xF0)
      return m_data[0] & 0xF0;
    return m_data[0];
  }


  MIDIEvent* MIDIEvent::get_next() const {
    return m_next;
  }


  const unsigned char* MIDIEvent::get_data() const {
    return m_data;
  }


  unsigned int MIDIEvent::get_size() const {
    return 3;
  }


  void MIDIEvent::set_type(unsigned char type) {
    m_data[0] = type;
  }


  void MIDIEvent::set_next(MIDIEvent* event) {
    m_next = event;
  }


}
