#include <iostream>

#include "noteevent.hpp"


using namespace std;


namespace Dino {


  NoteEvent::NoteEvent(unsigned char type, int stp, int val, int vel) 
    : MIDIEvent(type, val, vel),
      m_step(stp), 
      m_previous(NULL) {
    
  }
  
  
  NoteEvent::NoteEvent(unsigned char type, int stp, unsigned char data1, 
		       unsigned char data2) 
    : MIDIEvent(type, data1, data2),
      m_step(stp),
      m_previous(NULL) {
      
  }
   
  
  unsigned char NoteEvent::get_key() const {
    return m_data[1];
  }

  
  unsigned char NoteEvent::get_velocity() const {
    return m_data[2];
  }
  

  unsigned int NoteEvent::get_step() const {
    return m_step;
  }


  NoteEvent* NoteEvent::get_previous() const {
    return m_previous;
  }


  NoteEvent* NoteEvent::get_next() const {
    return static_cast<NoteEvent*>(MIDIEvent::get_next());
  }
  

  Note* NoteEvent::get_note() {
    return m_note;
  }

  
  void NoteEvent::set_key(unsigned char note) {
    m_data[1] = note;
  }

  
  void NoteEvent::set_velocity(unsigned char vel) {
    m_data[2] = vel;
  }
  

  void NoteEvent::set_step(unsigned int step) {
    m_step = step; 
  }


  void NoteEvent::set_previous(NoteEvent* event) {
    m_previous = event;
  }


  void NoteEvent::set_note(Note* note) {
    m_note = note;
  }

}
