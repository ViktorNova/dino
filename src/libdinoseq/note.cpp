#include "note.hpp"
#include "noteevent.hpp"


namespace Dino {

  
  Note::Note(NoteEvent* on, NoteEvent* off)
    : m_note_on(on),
      m_note_off(off) {
    
  }

  
  unsigned int Note::get_length() const {
    return m_note_off->get_step() + 1 - m_note_on->get_step();
  }
  
  
  unsigned char Note::get_key() const {
    return m_note_on->get_key();
  }


  unsigned char Note::get_velocity() const {
    return m_note_on->get_velocity();
  }
  
  
  unsigned int Note::get_step() const {
    return m_note_on->get_step();
  }


  NoteEvent* Note::get_note_on() {
    return m_note_on;
  }
  
  
  NoteEvent* Note::get_note_off() {
    return m_note_off;
  }
  
  
}
