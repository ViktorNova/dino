/****************************************************************************
    
    tempomap.hpp - A tempo map for the Dino sequencer
    
    Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
    
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
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef TEMPOMAP_HPP
#define TEMPOMAP_HPP

#include <sys/types.h>

#include <sigc++/sigc++.h>

#include "cdtree.hpp"


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
      
      unsigned long get_frame() const { return frame; }
      unsigned long get_beat() const { return beat; }
      unsigned int get_bpm() const { return bpm; }
      TempoChange* get_prev() const { return prev; }
      TempoChange* get_next() const { return next; }
      
    private:    
      
      friend class TempoMap;
      
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
    // XXX should return bool so the CommandProxy can tell if it worked or not
    /** Remove any tempo change that occurs at the given beat. */
    void remove_tempo_change(unsigned long beat);
    
    /** Get the BPM tempo and beat for a given frame number. */
    void get_beat(unsigned long frame, double& bpm, double& beat) const;
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
