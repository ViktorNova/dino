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

#include "debug.hpp"
#include "deleter.hpp"
#include "tempomap.hpp"


using namespace std;


namespace Dino {


  TempoMap::TempoMap(unsigned long frame_rate) 
    : m_tc_list(new TempoChange(0, 0, 100, NULL, NULL)),
      m_frame2tc(new CDTree<TempoChange*>(400000000)),
      m_frame_rate(frame_rate) {
  
    dbg1<<"Initialising tempo map"<<endl;
  
    const_cast<CDTree<TempoChange*>*>(m_frame2tc)->fill(0, 400000000, m_tc_list);
  }


  TempoMap::TempoChange* TempoMap::add_tempo_change(unsigned long beat, 
						    unsigned int bpm) {
    
    // XXX I don't understand how this works any more, it probably needs to be
    //     rewritten
    TempoChange* tc = NULL;
    TempoChange* iter;
  
    bool is_new = true;
  
    // insert the new tempo change
    for (iter = m_tc_list; iter != NULL; iter = iter->next) {
      if (iter->beat == beat) {
	if (iter->bpm == bpm)
	  return iter;
	tc = iter;
	tc->bpm = bpm;
	is_new = false;
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
  
    // update the framenumber->TempoChange mapping
    CDTree<TempoChange*>* tmp = new CDTree<TempoChange*>(400000000);
    unsigned long a, b;
    for (iter = m_tc_list; iter != NULL; iter = iter->next) {
      a = iter->frame;
      if (iter->next)
	b = iter->next->frame;
      else
	b = 400000000;
      tmp->fill(a, b, iter);
    }
  
    // CRITICAL SECTION
    /* m_frame2tc is accessed by get_bbt() which can be called by the 
       @c jack_timebase_callback function */
    CDTree<TempoChange*>* tmp2 = const_cast<CDTree<TempoChange*>*>(m_frame2tc);
    // let's hope that pointer assignment is atomic
    m_frame2tc = tmp;
    // we can't actually delete here, needs to be taken care of in the Deleter
    Deleter::queue(tmp2);
  
    // END OF CRITICAL SECTION
  
    if (is_new)
      signal_tempochange_added(beat);
    else
      signal_tempochange_changed(beat);
    
    return tc;
  }


  void TempoMap::remove_tempo_change(unsigned long beat) {
    // XXX I don't understand how this works any more, it probably needs to be
    //     rewritten

    if (beat == 0)
      return;
  
    bool removed = false;
    TempoChange* iter;
    for (iter = m_tc_list; iter != NULL; iter = iter->next) {
      if (iter->beat == beat) {
	if (iter->prev)
	  iter->prev->next = iter->next;
	if (iter->next)
	  iter->next->prev = iter->prev;
	delete iter;
	removed = true;
      }
    }

    // update the framenumber->TempoChange mapping
    CDTree<TempoChange*>* tmp = new CDTree<TempoChange*>(400000000);
    unsigned long a, b;
    for (iter = m_tc_list; iter != NULL; iter = iter->next) {
      a = iter->frame;
      if (iter->next)
	b = iter->next->frame;
      else
	b = 400000000;
      tmp->fill(a, b, iter);
    }
  
    // CRITICAL SECTION
    /* m_frame2tc is accessed by get_bbt() which can be called by the 
       @c jack_timebase_callback function */
    CDTree<TempoChange*>* tmp2 = const_cast<CDTree<TempoChange*>*>(m_frame2tc);
    // let's hope that pointer assignment is atomic
    m_frame2tc = tmp;
    // we can't actually delete here, needs to be taken care of in the Deleter
    Deleter::queue(tmp2);
  
    // END OF CRITICAL SECTION
  
    if (removed)
      signal_tempochange_removed(beat);
  }


  void TempoMap::get_beat(unsigned long frame, double& bpm, double& beat) const {
    TempoChange* tc = const_cast<CDTree<TempoChange*>*>(m_frame2tc)->get(frame);
    bpm = double(tc->bpm);
    beat = tc->beat + bpm * double(frame - tc->frame) / (60 * m_frame_rate);
    //beat = int32_t(beat_d);
    //tick = int32_t((beat_d - int(beat_d)) * ticks_per_beat);
  }
 

  unsigned long TempoMap::get_frame(int32_t beat, int32_t tick,
				    unsigned long ticks_per_beat) const {
    TempoChange* iter;
    for (iter = m_tc_list; iter != NULL; iter = iter->next) {
      if (!iter->next || iter->next->beat > (unsigned)beat)
	break;
    }
    return (unsigned long)(iter->frame + (beat + double(tick) / 
					  ticks_per_beat - iter->beat) *
			   (m_frame_rate * 60 / double(iter->bpm)));
  }


  const TempoMap::TempoChange* TempoMap::get_changes(unsigned long beat) const {
    const TempoChange* iter = m_tc_list;
    for ( ; iter != NULL; iter = iter->next) {
      if (!iter->next || iter->next->beat > beat)
	break;
    }
    return iter;
  }


}
