/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <larsl@users.sourceforge.net>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include "interpolatedevent.hpp"


namespace Dino {


  InterpolatedEvent::InterpolatedEvent(int start, int end, 
				       unsigned step, unsigned length) 
    : m_start(start),
      m_end(end),
      m_step(step),
      m_length(length) {

  }
  
  
  unsigned InterpolatedEvent::get_step() const {
    return m_step;
  }
  
  
  unsigned InterpolatedEvent::get_length() const {
    return m_length;
  }


  int InterpolatedEvent::get_start() const {
    return m_start;
  }
  
  
  int InterpolatedEvent::get_end() const {
    return m_end;
  }
  

  void InterpolatedEvent::set_step(unsigned int step) {
    m_step = step;
  }
  
  
  void InterpolatedEvent::set_length(unsigned int length) {
    m_length = length;
  }


  void InterpolatedEvent::set_start(int start) {
    m_start = start;
  }
  
  
  void InterpolatedEvent::set_end(int end) {
    m_end = end;
  }

  
}
