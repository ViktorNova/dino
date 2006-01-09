#include <iostream>

#include "basemidievent.hpp"


using namespace std;


namespace Dino {


  BaseMIDIEvent BaseMIDIEvent::AllNotesOff(0xB0, 123, 0);
  BaseMIDIEvent BaseMIDIEvent::ChannelVolume(0xB0, 7, 10);

  
  unsigned char BaseMIDIEvent::get_type() const {
    return m_data[0];
  }


  unsigned char BaseMIDIEvent::get_note() const {
    return m_data[1];
  }


  unsigned char BaseMIDIEvent::get_velocity() const {
    return m_data[2];
  }


  unsigned char BaseMIDIEvent::get_value() const {
    return m_data[2];
  }
  
  
  BaseMIDIEvent* BaseMIDIEvent::get_next() const {
    return m_next;
  }


  const unsigned char* BaseMIDIEvent::get_data() const {
    return m_data;
  }


  unsigned int BaseMIDIEvent::get_size() const {
    return 3;
  }


  unsigned char BaseMIDIEvent::get_channel() const {
    return m_data[0] & 0xF;
  }


  void BaseMIDIEvent::set_type(unsigned char type) {
    m_data[0] = type;
  }


  void BaseMIDIEvent::set_note(unsigned char note) {
    m_data[1] = note;
  }


  void BaseMIDIEvent::set_velocity(unsigned char vel) {
    m_data[2] = vel;
  }


  void BaseMIDIEvent::set_value(unsigned char value) {
    m_data[2] = value;
  }


  void BaseMIDIEvent::set_next(BaseMIDIEvent* event) {
    m_next = event;
  }


  void BaseMIDIEvent::set_channel(unsigned char channel) {
    m_data[0] &= 0xF0;
    m_data[0] |= channel & 0x0F;
  }


}
