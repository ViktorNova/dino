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

#ifndef INTERPOLATEDEVENT_HPP
#define INTERPOLATEDEVENT_HPP


namespace Dino {
  
  
  /** This class represents segments in Controllers, with a start value and
      start step and an end value and end step. The sequencer should 
      use linear interpolation to get the intermediate values. */
  class InterpolatedEvent {
  public:
    
    InterpolatedEvent(int start, int end, unsigned step, unsigned length);
    
    unsigned get_step() const;
    unsigned get_length() const;
    int get_start() const;
    int get_end() const;

    void set_step(unsigned step);
    void set_length(unsigned length);
    void set_start(int start);
    void set_end(int end);
    
  protected:
    
    int m_start;
    int m_end;
    unsigned m_step;
    unsigned m_length;
    
  };


}


#endif
