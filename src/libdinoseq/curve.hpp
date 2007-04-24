/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#ifndef CURVE_HPP
#define CURVE_HPP

#include <string>
#include <vector>


namespace Dino {

  
  class ControllerInfo;
  class InterpolatedEvent;
  
  
  /** This class represents a sequence of MIDI CC events used in a Pattern. */
  class Curve {
  public:

    /** Create a new controller with the given parameters. */
    Curve(const ControllerInfo& info, unsigned int size);
    ~Curve();
    
    /// @name Accessors
    //@{
    /** Return the active event at the given step. */
    const InterpolatedEvent* get_event(unsigned int step) const;
    /** Return the number of time steps in this controller. */
    unsigned int get_size() const;
    /** Return the ControllerInfo struct for this curve. */
    const ControllerInfo& get_info() const;
    //@}

    /// @name Mutators
    //@{
    /** Add a control point to this controller. */
    void add_point(unsigned int step, int value);
    /** Remove a control point from this controller. */
    void remove_point(unsigned int step);
    //@}

  private:
    
    const ControllerInfo& m_info;
    std::string m_name;
    std::vector<InterpolatedEvent*> m_events;
    long m_param;
    int m_min;
    int m_max;
    
  };

  
}


#endif
