/****************************************************************************
    
    tempomap.hpp - A tempo map for the Dino sequencer
    
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

#ifndef TEMPOMAP_HPP
#define TEMPOMAP_HPP

#include <sys/types.h>


/** This class maps frame numbers to beats and ticks, and beats and ticks
    to frame numbers. It has functions for removing and adding tempo 
    changes on whole beats. */
class TempoMap {
public:
  
  TempoMap(unsigned long frame_rate = 48000); 

  void add_tempo_change(unsigned long beat, unsigned int bpm);
  void remove_tempo_change(unsigned long beat);
  
  void get_bbt(unsigned long frame, unsigned long ticks_per_beat,
	       double& bpm, int32_t& beat, int32_t& tick) const;
  unsigned long get_frame(int32_t beat, int32_t tick,
			  unsigned long ticks_per_beat) const;
  
protected:
  
};


#endif
