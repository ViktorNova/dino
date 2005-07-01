/****************************************************************************
    
    event.hpp - A MIDI event implementation for the Dino sequencer
    
    Copyright (C) 2005  Lars Luthman <larsl@users.sourceforge.net>
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef EVENT_HPP
#define EVENT_HPP


class Event {
public:
  
  unsigned char get_type() const;
  unsigned char get_note() const;
  unsigned char get_velocity() const;
  Event* get_next() const;
  Event* get_previous() const;
  Event* get_assoc() const;
  
protected:
  unsigned int m_step;
  Event* m_next;
  Event* m_previous;
  Event* m_assoc;
  unsigned char m_data[4];
};


#endif
