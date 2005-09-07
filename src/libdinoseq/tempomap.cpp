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

#include "tempomap.hpp"


using namespace std;


TempoMap::TempoMap(unsigned long frame_rate) 
  : m_tc_list(new TempoChange(0, 0, 100, NULL, NULL)),
    m_frame2tc(new CDTree<TempoChange*>(400000000)),
    m_frame_rate(frame_rate) {
  
  const_cast<CDTree<TempoChange*>*>(m_frame2tc)->fill(0, 400000000, m_tc_list);
  
  
  /*
  TempoChange* iter;
  
  for (iter = m_tc_list; iter != NULL; iter = iter->next)
    cerr<<" -> "<<iter->beat<<": ("<<iter->bpm<<", "<<iter->frame<<")";
  cerr<<endl;
  
  add_tempo_change(0, 145);

  for (iter = m_tc_list; iter != NULL; iter = iter->next)
    cerr<<" -> "<<iter->beat<<": ("<<iter->bpm<<", "<<iter->frame<<")";
  cerr<<endl;
  
  add_tempo_change(10, 150);

  for (iter = m_tc_list; iter != NULL; iter = iter->next)
    cerr<<" -> "<<iter->beat<<": ("<<iter->bpm<<", "<<iter->frame<<")";
  cerr<<endl;

  add_tempo_change(4, 132);
  
  for (iter = m_tc_list; iter != NULL; iter = iter->next)
    cerr<<" -> "<<iter->beat<<": ("<<iter->bpm<<", "<<iter->frame<<")";
  cerr<<endl;
  */  
  
  
}


void TempoMap::add_tempo_change(unsigned long beat, unsigned int bpm) {
  TempoChange* tc = NULL;
  TempoChange* iter;
  
  // insert the new tempo change
  for (iter = m_tc_list; iter != NULL; iter = iter->next) {
    if (iter->beat == beat) {
      tc = iter;
      tc->bpm = bpm;
      break;
    }
    else if (iter->beat < beat && !iter->next) {
      unsigned long frame;
      frame = (unsigned long)(iter->frame + (beat - iter->beat) * 
			      m_frame_rate * 60 / double(iter->bpm));
      tc = new TempoChange(frame, beat, bpm, iter, NULL);
      tc->prev->next = tc;
      break;
    }
    else if (iter->beat < beat && iter->next && iter->next->beat > beat) {
      unsigned long frame;
      frame = (unsigned long)(iter->frame + (beat - iter->beat) * 
			      m_frame_rate * 60 / double(iter->bpm));
      tc = new TempoChange(frame, beat, bpm, iter, iter->next);
      tc->prev->next = tc;
      tc->next->prev = tc;
      break;
    }
  }
  
  // update the frame numbers for all subsequent tempo changes
  if (tc->next) {
    unsigned long dframe = 
      (unsigned long)(tc->frame + (tc->next->beat - beat) * 
		      m_frame_rate * 60 / double(tc->bpm)) - tc->next->frame;
    for (iter = tc->next; iter != NULL; iter = iter->next)
      iter->frame += dframe;
  }  
}


void TempoMap::remove_tempo_change(unsigned long beat) {
  if (beat == 0)
    return;
  TempoChange* iter;
  for (iter = m_tc_list; iter != NULL; iter = iter->next) {
    if (iter->beat == beat) {
      if (iter->prev)
	iter->prev->next = iter->next;
      if (iter->next)
	iter->next->prev = iter->prev;
      delete iter;
    }
  }
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


const TempoMap::TempoChange* TempoMap::get_changes(unsigned long beat) const {
  const TempoChange* iter = m_tc_list;
  for ( ; iter != NULL; iter = iter->next) {
    if (!iter->next || iter->next->beat > beat)
      break;
  }
  return iter;
}
