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

#include "event.hpp"
#include "eventlist.hpp"


namespace Dino {


  /** Insert a new event into the event list. */
  Event* EventList::insert(Event* event) {
    
    // initialise the pointers to previous and next event on all levels
    Event* prev[Event::Levels];
    Event* next[Event::Levels];
    for (unsigned i = 0; i < Event::Levels; ++i) {
      prev[i] = &m_head;
      next[i] = m_head.m_next[i];
      event->m_prev[i] = 0;
      event->m_next[i] = 0;
    }
    
    // find where to insert the event
    for (int level = Event::Levels - 1; level >= 0; --level) {
      while (next[level] && *next[level] < *event) {
	prev[level] = next[level];
	next[level] = next[level]->m_next[level];
      }
    }
    
    // insert it
    int r = std::rand();
    int t = RAND_MAX / Event::Scale;
    for (int level = 0; level < Event::Levels; ++level) {
      event->m_next[level] = next[level];
      event->m_prev[level] = prev[level];
      if (next[level])
	next[level]->m_prev[level] = event;
      prev[level]->m_next[level] = event;
      if (r > t)
	break;
      t /= Event::Scale;
    }
    
    return event;
  }
  
  
  /** Remove a event from the list, but don't delete it. */
  void EventList::erase(Event* event) {
    for (int level = 0; level < Event::Levels; ++level) {
      if (!event->m_prev[level])
	break;
      event->m_prev[level]->m_next[level] = event->m_next[level];
      if (event->m_next[level])
	event->m_next[level]->m_prev[level] = event->m_prev[level];
      event->m_prev[level] = 0;
      // keep the next pointer since the sequencer may be visiting
      // this event right now
    }
  }
  
  
  /** Find the first event at or after the given time. */
  Event* EventList::find(const SongTime& time) {
    
    // initialise the pointers to previous and next events on all levels
    Event* prev[Event::Levels];
    Event* next[Event::Levels];
    for (unsigned i = 0; i < Event::Levels; ++i) {
      prev[i] = &m_head;
      next[i] = m_head.m_next[i];
    }
    
    // find the event
    for (int level = Event::Levels - 1; level >= 0; --level) {
      while (next[level] && 
	     next[level]->get_time() < time) {
	prev[level] = next[level];
	next[level] = next[level]->m_next[level];
      }
    }
    
    return next[0];
  }
  
  
  Event* EventList::get_start() {
    return m_head.m_next[0];
  }
  

}
