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

#ifndef EVENTLIST_HPP
#define EVENTLIST_HPP

#include <cstdlib>

#include "event.hpp"
#include "songtime.hpp"


namespace Dino {

  
  template <unsigned int Levels, unsigned int Scale>
  class EventList {
  public:
    
    struct Node {
      
      Node(const SongTime& t = SongTime(), Event* e = 0)
	: time(t),
	  event(e) {
	
      }
      
      SongTime time;
      Event* event;
      
      Node* prev[Levels];
      Node* next[Levels];
      Node* buddy;
      
    };
    
    
    EventList() {
      
    };
    
    void insert(Node* node) {
      
      // initialise the pointers to previous and next nodes on all levels
      Node* prev[Levels];
      Node* next[Levels];
      for (unsigned i = 0; i < Levels; ++i) {
	prev[i] = &m_head;
	next[i] = m_head.m_next[i];
	node->prev[i] = 0;
	node->next[i] = 0;
      }
      
      // find where to insert the node
      for (int level = Levels - 1; level >= 0; --level) {
	while (next[level] && next[level]->time < node->time) {
	  prev[level] = next[level];
	  next[level] = next[level]->next[level];
	}
      }
      
      // insert it
      int r = std::rand();
      int t = RAND_MAX / Scale;
      for (int level = 0; level < Levels; ++level) {
	node->next[level] = next[level];
	node->prev[level] = prev[level];
	if (next[level])
	  next[level]->prev[level] = node;
	prev[level]->next[level] = node;
	if (r > t)
	  break;
	t /= Scale;
      }
    }
    
    
    void erase(Node* node) {
      for (int level = 0; level < Levels; ++level) {
	if (!node->prev[level])
	  break;
	node->prev[level]->next[level] = node->next[level];
	if (node->next[level])
	  node->next[level]->prev[level] = node->prev[level];
	node->prev[level] = 0;
	// keep the next pointer since the sequencer may be visiting
	// this node right now
      }
    }
    
    
    Node* find(const SongTime& time) {
      
      
    }
    

  private:
    
    Node m_head;
    
  };


}


#endif
