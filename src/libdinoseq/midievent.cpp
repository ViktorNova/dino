#include <iostream>

#include "midievent.hpp"


using namespace std;


namespace Dino {


  MIDIEvent MIDIEvent::AllNotesOff(0xB0, 0, 123, 0);
  MIDIEvent MIDIEvent::ChannelVolume(0xB0, 0, 7, 10);

  
  unsigned int MIDIEvent::get_length() const {
    return m_length;
  }


  unsigned int MIDIEvent::get_step() const {
    return m_step;
  }


  unsigned char MIDIEvent::get_type() const {
    return m_data[0];
  }


  unsigned char MIDIEvent::get_note() const {
    return m_data[1];
  }


  unsigned char MIDIEvent::get_velocity() const {
    return m_data[2];
  }


  MIDIEvent* MIDIEvent::get_next() const {
    return m_next;
  }


  MIDIEvent* MIDIEvent::get_previous() const {
    return m_previous;
  }


  MIDIEvent* MIDIEvent::get_assoc() const {
    return m_assoc;
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


  void MIDIEvent::set_length(unsigned int length) {
    m_length = length;
  }


  void MIDIEvent::set_step(unsigned int step) {
    m_step = step; 
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


  void MIDIEvent::set_next(MIDIEvent* event) {
    m_next = event;
  }


  void MIDIEvent::set_previous(MIDIEvent* event) {
    m_previous = event;
  }


  void MIDIEvent::set_assoc(MIDIEvent* event) {
    m_assoc = event;
  }


  void MIDIEvent::set_channel(unsigned char channel) {
    m_data[0] &= 0xF0;
    m_data[0] |= channel & 0x0F;
  }


}
