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

#include <sigc++/sigc++.h>

#include "cdtree.hpp"


using namespace sigc;


namespace Dino {


  /** This class maps frame numbers to beats and ticks, and beats and ticks
      to frame numbers. It has functions for removing and adding tempo 
      changes on whole beats. */
  class TempoMap {
  public:
  
    class TempoChange {
    public:

      TempoChange(unsigned long nframe, unsigned long nbeat, unsigned int nbpm,
		  TempoChange* nprev, TempoChange* nnext)
	: frame(nframe), beat(nbeat), bpm(nbpm), prev(nprev), next(nnext) {
    
      }
    
      unsigned long frame;
      unsigned long beat;
      unsigned int bpm;
      TempoChange* prev;
      TempoChange* next;
    };
  
    /** Create a new tempo map for the given frame rate. */
    TempoMap(unsigned long frame_rate = 48000); 
    
    /** Add a new tempo change. */
    TempoChange* add_tempo_change(unsigned long beat, unsigned int bpm);
    /** Remove any tempo change that occurs at the given beat. */
    void remove_tempo_change(unsigned long beat);
    
    /** Get the BPM tempo, beat, and tick for a given frame number. */
    void get_bbt(unsigned long frame, unsigned long ticks_per_beat,
		 double& bpm, int32_t& beat, int32_t& tick) const;
    /** Get the frame number for the given beat and tick. */
    unsigned long get_frame(int32_t beat, int32_t tick,
			    unsigned long ticks_per_beat) const;
    /** Get a linked list of all tempo changes. */
    const TempoChange* get_changes(unsigned long beat) const;
  
    sigc::signal<void, int> signal_tempochange_added;
    sigc::signal<void, int> signal_tempochange_removed;
    sigc::signal<void, int> signal_tempochange_changed;
  
  private:
  
    TempoChange* m_tc_list;
    volatile CDTree<TempoChange*>* m_frame2tc;
  
    unsigned long m_frame_rate;
  };


}


#endif
