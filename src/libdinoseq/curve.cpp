/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
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
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include <cassert>

#include "controller_numbers.hpp"
#include "controllerinfo.hpp"
#include "curve.hpp"
#include "deleter.hpp"
#include "interpolatedevent.hpp"
#include "midibuffer.hpp"
#include "songtime.hpp"


namespace Dino {


  Curve::Curve(const ControllerInfo& info, unsigned int size)
    : m_info(info),
      m_events(size, 0) {

  }
  
  
  Curve::~Curve() {
    InterpolatedEvent* previous = 0;
    for (unsigned i = 0; i < m_events.size(); ++i) {
      if (m_events[i] != 0 && m_events[i] != previous)
        delete m_events[i];
      previous = m_events[i];
    }
  }

  
  unsigned int Curve::get_size() const {
    return m_events.size();
  }


  const ControllerInfo& Curve::get_info() const {
    return m_info;
  }


  void Curve::add_point(unsigned int step, int value) {
    assert(step <= m_events.size());
    assert(value >= m_info.get_min());
    assert(value <= m_info.get_max());
    
    // change the last point in the curve if there is an event
    if (step == m_events.size() && m_events[step - 1]) {
      m_events[step - 1]->set_end(value);
      m_signal_point_changed(step, value);
      return;
    }
      
    // if an old event is going on at this step, handle it
    InterpolatedEvent* old_event = m_events[step];
    if (old_event != 0) {
      if (old_event->get_step() == step) {
        old_event->set_start(value);
	m_signal_point_changed(step, value);
        if (step > 0 && m_events[step - 1]) {
          m_events[step - 1]->set_end(value);
          m_events[step - 1]->set_length(step - m_events[step - 1]->get_step());
        }
      }
      else {
        unsigned int length = old_event->get_step() + 
          old_event->get_length() - step;
        int end = old_event->get_end();
        old_event->set_length(step - old_event->get_step());
        old_event->set_end(value);
        m_events[step] = new InterpolatedEvent(value, end, step, length);
        for (unsigned i = step + 1; i < step + length; ++i)
          m_events[i] = m_events[step];
	m_signal_point_added(step, value);
      }
    }
    
    // if not, we are the first event
    else {
      unsigned i;
      for (i = step; i < m_events.size() && m_events[i] == 0; ++i);
      int end = value;
      if (i < m_events.size())
        end = m_events[i]->get_start();
      if (step > 0 && m_events[step - 1]) {
        m_events[step - 1]->set_end(value);
        m_events[step - 1]->set_length(step - m_events[step - 1]->get_step());
      }
      m_events[step] = new InterpolatedEvent(value, end, step, i - step);
      for (unsigned j = step + 1; j < i; ++j)
        m_events[j] = m_events[step];
      m_signal_point_added(step, value);
    }
  }


  void Curve::remove_point(unsigned int step) {
    assert(step < m_events.size());
    
    InterpolatedEvent* event = m_events[step];
    if (!event || event->get_step() != step)
      return;
    
    for (unsigned i = step; i < step + event->get_length(); ++i)
      m_events[i] = 0;
    
    if (event && event->get_step() == step) {
      if (step > 0 && m_events[step - 1]) {
        m_events[step - 1]->set_end(event->get_end());
        m_events[step - 1]->set_length(step + event->get_length() - 
                                       m_events[step - 1]->get_step());
        for (unsigned i = step; i < step + event->get_length(); ++i)
          m_events[i] = m_events[step - 1];
      }
    }
    
    Deleter::queue(event);
    m_signal_point_removed(step);
  }


  const InterpolatedEvent* Curve::get_event(unsigned int step) const {
    assert(step < m_events.size());
    return m_events[step];
  }


  bool Curve::write_events(MIDIBuffer& buffer, double step, 
			   const SongTime& beat_time, 
			   unsigned char channel) const {
    channel &= 0x0F;
    const InterpolatedEvent* event = get_event(unsigned(step));
    if (event) {
      unsigned char* data = buffer.reserve(beat_time, 3);
      if (!data)
	return false;
      if (data && is_cc(get_info().get_number())) {
	data[0] = 0xB0 | channel;
	data[1] = cc_number(get_info().get_number());
	data[2] = (unsigned char)
	  (event->get_start() + (step - event->get_step()) *
	   ((event->get_end() - event->get_start()) /
	    double(event->get_length())));
      }
      else if (data && is_pbend(get_info().get_number())) {
	data[0] = 0xE0 | channel;
	int value = int(event->get_start() + 
			(beat_time - event->get_step()) *
			((event->get_end() - event->get_start()) /
			 double(event->get_length())));
	data[1] = (value + 8192) & 0x7F;
	data[2] = ((value + 8192) >> 7) & 0x7F;
      }
    }
    
    return true;
  }


  sigc::signal<void, int, int>& Curve::signal_point_added() const {
    return m_signal_point_added;
  }


  sigc::signal<void, int, int>& Curve::signal_point_changed() const {
    return m_signal_point_changed;
  }


  sigc::signal<void, int>& Curve::signal_point_removed() const {
    return m_signal_point_removed;
  }



}
