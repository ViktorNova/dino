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

#include <iostream>

#include "cdtree.hpp"
#include "tempomap.hpp"


using namespace std;


TempoMap::TempoMap(unsigned long frame_rate) {
  
  CDTree<int> cdtree(100000000);
  
  cerr<<"[";
  for (int i = 0; i < 20; ++i)
    cerr<<" "<<cdtree.get(i);
  cerr<<" ]"<<endl;
  
  cdtree.fill(4, 5);
  
  cerr<<cdtree.get(9457982)<<" "<<cdtree.get(218347)<<endl;

}


void TempoMap::add_tempo_change(unsigned long beat, unsigned int bpm) {
  
}


void TempoMap::remove_tempo_change(unsigned long beat) {

}


void TempoMap::get_bbt(unsigned long frame, unsigned long ticks_per_beat,
		       double& bpm, int32_t& beat, int32_t& tick) const {
  bpm = 100.0;
  double beat_d = bpm * double(frame) / (60.0 * ticks_per_beat);
  beat = int32_t(beat_d);
  tick = int32_t((beat_d - int(beat_d)) * 10000);
}
 

unsigned long TempoMap::get_frame(int32_t beat, int32_t tick,
				  unsigned long ticks_per_beat) const {
  return (unsigned long)(beat * 60 * ticks_per_beat / 100.0);
}

