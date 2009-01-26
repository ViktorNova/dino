/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2009  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, 
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#ifndef EVENT_HPP
#define EVENT_HPP

#include "songtime.hpp"


namespace Dino {


  class Pattern;
  

  class Event {
  public:
    
    Event()
      : m_type(0),
	m_time(0, 0),
	m_buddy(0),
	m_prev_similar(0),
	m_next_similar(0) {
      for (unsigned l = 0; l < Levels; ++l)
	m_prev[l] = m_next[l] = 0;
    }

    
    Event(uint32_t type, const SongTime& time, 
	  unsigned char data1, unsigned char data2 = 0,
	  unsigned char data3 = 0, unsigned char data4 = 0)
      : m_type(type),
	m_time(time),
	m_buddy(0),
	m_prev_similar(0),
	m_next_similar(0) {
      m_data[0] = data1;
      m_data[1] = data2;
      m_data[2] = data3;
      m_data[3] = data4;
      for (unsigned l = 0; l < Levels; ++l)
	m_prev[l] = m_next[l] = 0;
    }
    
    uint32_t get_type() const { return m_type; }
    
    const unsigned char* get_data() const { return m_data; }
    
    Event* get_buddy() { return m_buddy; }
    
    bool operator<=(const Event& e) {
      return (m_time <= e.get_time() ||
	      (m_time == e.get_time() && 
	       m_type <= e.get_type()) ||
	      (m_time == e.get_time() && 
	       m_type == e.get_type() &&
	       m_data[0] <= e.get_data()[0]) ||
	      (m_time == e.get_time() &&
	       m_type == e.get_type() &&
	       m_data[0] <= e.get_data()[0] &&
	       m_data[1] <= e.get_data()[1]) ||
	      (m_time == e.get_time() &&
	       m_type == e.get_type() &&
	       m_data[0] <= e.get_data()[0] &&
	       m_data[1] <= e.get_data()[1] &&
	       m_data[2] <= e.get_data()[2]) ||
	      (m_time == e.get_time() &&
	       m_type == e.get_type() &&
	       m_data[0] <= e.get_data()[0] &&
	       m_data[1] <= e.get_data()[1] &&
	       m_data[2] <= e.get_data()[2] &&
	       m_data[3] <= e.get_data()[3]));
    }
    
    bool operator<(const Event& e) {
      return (m_time < e.get_time() ||
	      (m_time == e.get_time() && 
	       m_type < e.get_type()) ||
	      (m_time == e.get_time() && 
	       m_type == e.get_type() &&
	       m_data[0] < e.get_data()[0]) ||
	      (m_time == e.get_time() &&
	       m_type == e.get_type() &&
	       m_data[0] < e.get_data()[0] &&
	       m_data[1] < e.get_data()[1]) ||
	      (m_time == e.get_time() &&
	       m_type == e.get_type() &&
	       m_data[0] < e.get_data()[0] &&
	       m_data[1] < e.get_data()[1] &&
	       m_data[2] < e.get_data()[2]) ||
	      (m_time == e.get_time() &&
	       m_type == e.get_type() &&
	       m_data[0] < e.get_data()[0] &&
	       m_data[1] < e.get_data()[1] &&
	       m_data[2] < e.get_data()[2] &&
	       m_data[3] < e.get_data()[3]));
    }
        
    const SongTime& get_time() const { return m_time; }
    
    unsigned char* get_data() { return m_data; }
    
    unsigned char get_key() { return m_data[0]; }
    
    unsigned char get_velocity() { return m_data[1]; }
    
    SongTime get_length() { 
      if (!m_buddy)
	return SongTime(0, 0);
      return m_buddy->get_time() - get_time();
    }
	
    
    bool set_velocity(unsigned char velocity) { 
      if (velocity > 128)
	return false;
      m_data[1] = velocity;
      return true;
    }
    
    void set_buddy(Event* buddy) { m_buddy = buddy; }
    
  public:
    
    static uint32_t note_on() { return 1; }

    static uint32_t note_off() { return 0; }
    
    static bool is_note_on(uint32_t type) { return (type == 0); }

    static bool is_note_off(uint32_t type) { return (type == 1); }
    
  public:

    static const unsigned int Levels = 4;
    static const unsigned int Scale = 4;

  private:
    
    friend class EventList;
    friend class Pattern;
    
    uint32_t m_type;
    SongTime m_time;
    unsigned char m_data[4];

    Event* m_prev[Levels];
    Event* m_next[Levels];
    Event* m_prev_similar;
    Event* m_next_similar;
    Event* m_buddy;

  };


}


#endif
