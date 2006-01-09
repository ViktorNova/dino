#include <iostream>

#include "noteevent.hpp"


using namespace std;


namespace Dino {


  unsigned int NoteEvent::get_length() const {
    return m_length;
  }


  unsigned int NoteEvent::get_step() const {
    return m_step;
  }


  NoteEvent* NoteEvent::get_previous() const {
    return m_previous;
  }


  NoteEvent* NoteEvent::get_assoc() const {
    return m_assoc;
  }


  void NoteEvent::set_length(unsigned int length) {
    m_length = length;
  }


  void NoteEvent::set_step(unsigned int step) {
    m_step = step; 
  }


  void NoteEvent::set_previous(NoteEvent* event) {
    m_previous = event;
  }


  void NoteEvent::set_assoc(NoteEvent* event) {
    m_assoc = event;
  }


}
