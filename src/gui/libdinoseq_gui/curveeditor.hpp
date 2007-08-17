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

#ifndef CURVEEDITOR_HPP
#define CURVEEDITOR_HPP

#include <utility>

#include <gtkmm.h>


namespace Dino {
  class CommandProxy;
  class Curve;
}


class CurveEditor : public Gtk::DrawingArea {
public:
  
  CurveEditor(Dino::CommandProxy& proxy);
  
  void set_curve(int track, int pattern, Dino::Curve* curve);
  void set_step_width(int width);
  void set_alternation(int k);
  
  sigc::signal<void, const std::string&>& signal_status();
  
protected:
  
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  
  int value2ypix(int value);
  int ypix2value(int value);
  int step2xpix(int value);
  int xpix2step(int value);
  
  Glib::RefPtr<Gdk::GC> m_gc;
  Gdk::Color m_bg_colour1, m_bg_colour2, m_grid_colour, 
    m_edge_colour, m_fg_colour;
  
  unsigned m_step_width;
  unsigned m_alternation;
  
  int m_track;
  int m_pattern;
  Dino::Curve* m_curve;
  Dino::CommandProxy& m_proxy;
  
  int m_drag_step;
  
  sigc::signal<void, const std::string&> m_signal_status;
};


#endif
