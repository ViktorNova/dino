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


  unsigned char MIDIEvent::get_note() const {
    return m_data[1];
  }


  unsigned char MIDIEvent::get_velocity() const {
    return m_data[2];
  }


  unsigned char MIDIEvent::get_value() const {
    return m_data[2];
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


  unsigned char MIDIEvent::get_channel() const {
    return m_data[0] & 0xF;
  }


  void MIDIEvent::set_type(unsigned char type) {
    m_data[0] = type;
  }


  void MIDIEvent::set_note(unsigned char note) {
    m_data[1] = note;
  }


  void MIDIEvent::set_velocity(unsigned char vel) {
    m_data[2] = vel;
  }


  void MIDIEvent::set_value(unsigned char value) {
    m_data[2] = value;
  }


  void MIDIEvent::set_next(MIDIEvent* event) {
    m_next = event;
  }


  void MIDIEvent::set_channel(unsigned char channel) {
    m_data[0] &= 0xF0;
    m_data[0] |= channel & 0x0F;
  }


}
