/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006 - 2009  Lars Luthman <lars.luthman@gmail.com>
   
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

#ifndef CURVEEDITOR2_HPP
#define CURVEEDITOR2_HPP

#include <utility>

#include <gtkmm.h>

#include "songtime.hpp"


namespace Dino {
  class CommandProxy;
  class ControllerInfo;
  class Curve;
  class Pattern;
  class Track;
}


class CurveEditor2 : public Gtk::DrawingArea {
public:
  
  CurveEditor2(Dino::CommandProxy& proxy);
  
  void set_curve(Dino::Track& track, Dino::Pattern& pattern, 
		 Dino::ControllerInfo& controller);
  void unset_curve();
  void set_alternation(unsigned alternation);
  
  sigc::signal<void, const std::string&>& signal_status();
  
protected:
  
  void draw_background(Glib::RefPtr<Gdk::Window>& win);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  Dino::SongTime pixel2time(int x);
  int pixel2value(int y);
  int time2pixel(const Dino::SongTime& time);
  int value2pixel(int value);
  
  Glib::RefPtr<Gdk::GC> m_gc;
  Gdk::Color m_bg_colour1, m_bg_colour2, m_grid_colour, 
    m_edge_colour, m_fg_colour;
  
  Dino::SongTime::Tick m_ticks_per_pixel;
  unsigned m_height;
  unsigned m_alternation;
  
  Dino::Track* m_track;
  Dino::Pattern* m_pattern;
  const Dino::ControllerInfo* m_curve;
  Dino::CommandProxy& m_proxy;
  
  sigc::signal<void, const std::string&> m_signal_status;
};


#endif
