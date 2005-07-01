#include "event.hpp"

  
unsigned char Event::get_type() const {
  return m_data[0];
}


unsigned char Event::get_note() const {
  return m_data[1];
}


unsigned char Event::get_velocity() const {
  return m_data[2];
}


Event* Event::get_next() const {
  return m_next;
}


Event* Event::get_previous() const {
  return m_previous;
}


Event* Event::get_assoc() const {
  return m_assoc;
}
