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

#include <iostream>

#include "controllerinfo.hpp"
#include "curveeditor2.hpp"
#include "pattern.hpp"


using namespace Dino;
using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace std;


CurveEditor2::CurveEditor2(Dino::CommandProxy& proxy)
  : m_bg_colour1("#FFFFFF"),
    m_bg_colour2("#EAEAFF"),
    m_grid_colour("#9C9C9C"),
    m_edge_colour("#000000"),
    m_fg_colour("#008000"),
    m_ticks_per_pixel(SongTime::ticks_per_beat() / 64),
    m_height(68),
    m_alternation(1),
    m_track(0),
    m_pattern(0),
    m_curve(0),
    m_proxy(proxy) {
  
  RefPtr<Colormap> cmap = Colormap::get_system();
  cmap->alloc_color(m_bg_colour1);
  cmap->alloc_color(m_bg_colour2);
  cmap->alloc_color(m_grid_colour);
  cmap->alloc_color(m_edge_colour);
  cmap->alloc_color(m_fg_colour);
  
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  set_size_request(time2pixel(SongTime(8, 0)), m_height);
}

  
void CurveEditor2::set_curve(Track& track, Pattern& pattern, 
			     ControllerInfo& controller) {
  m_track = &track;
  m_pattern = &pattern;
  m_curve = &controller;
  set_size_request(time2pixel(m_pattern->get_length()) + 1, m_height);
}


void CurveEditor2::unset_curve() {
  m_track = 0;
  m_pattern = 0;
  m_curve = 0;
  set_size_request(time2pixel(SongTime(8, 0)), m_height);
}


void CurveEditor2::set_alternation(unsigned alternation) {
  m_alternation = alternation;
  queue_draw();
}
 

sigc::signal<void, const std::string&>& CurveEditor2::signal_status() {
  return m_signal_status;
}

  
void CurveEditor2::draw_background(Glib::RefPtr<Gdk::Window>& win) {
  
  // draw background and vertical grid
  bool bg = true;
  int beat_width = time2pixel(SongTime(1, 0));
  SongTime::Beat b;
  //  for (b = 0; b < m_pattern->get_length().get_beat(); ++b) {
  for (b = 0; b < 8; ++b) {
    int x = time2pixel(SongTime(b, 0));
    m_gc->set_foreground( bg ? m_bg_colour1 : m_bg_colour2);
    bg = !bg;
    win->draw_rectangle(m_gc, true, x, 0, beat_width, m_height);
    m_gc->set_foreground(m_grid_colour);
    win->draw_line(m_gc, x, 0, x, m_height);
  }
  //SongTime::Tick t = m_pattern->get_length().get_tick();
  SongTime::Tick t = 0;
  if (t > 0) {
    m_gc->set_foreground( bg ? m_bg_colour1 : m_bg_colour2);
    win->draw_rectangle(m_gc, true, time2pixel(SongTime(b, 0)),
			0, time2pixel(SongTime(0, t)), m_height);
  }
  //int x = time2pixel(m_pattern->get_length());
  int x = time2pixel(SongTime(8, 0));
  m_gc->set_foreground(m_grid_colour);
  win->draw_line(m_gc, x, 0, x, m_height);
  

}


bool CurveEditor2::on_button_press_event(GdkEventButton* event) {
  
  if (!m_pattern)
    return true;
  
  SongTime time = pixel2time(event->x);
  int value = pixel2value(event->y);
  
  cerr<<"Press: "<<time.get_beat()<<":"<<time.get_tick()<<", "<<value<<endl;
  
  if (time < SongTime(0, 0) && time >= m_pattern->get_length() &&
      value < m_curve->get_min() && value > m_curve->get_max())
    return true;
  
  m_pattern->add_curve_point(m_curve->get_number(), time, value);
  
  queue_draw();
  
  return true;
}


bool CurveEditor2::on_button_release_event(GdkEventButton* event) {
  return true;
}


bool CurveEditor2::on_motion_notify_event(GdkEventMotion* event) {
  return true;
}


void CurveEditor2::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_colour1);
  m_gc->set_foreground(m_fg_colour);
  win->clear();
}


bool CurveEditor2::on_expose_event(GdkEventExpose* event) {
  
  cerr<<__PRETTY_FUNCTION__<<endl;
  
  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (!m_pattern)
    return true;
  
  draw_background(win);
  
  Pattern::CurveIterator iter;
  uint32_t number = m_curve->get_number();
  for (iter = m_pattern->curves_begin(number); 
       iter != m_pattern->curves_end(number); ++iter) {
    
    cerr<<"event at "
	<<iter->get_time().get_beat()<<":"<<iter->get_time().get_tick()
	<<", "<<iter->get_controller_value()<<endl;
    
    int x = time2pixel(iter->get_time());
    int y = value2pixel(iter->get_controller_value());
    m_gc->set_foreground(m_fg_colour);
    win->draw_rectangle(m_gc, true, x - 2, y - 2, 5, 5);
    m_gc->set_foreground(m_edge_colour);
    win->draw_rectangle(m_gc, false, x - 2, y - 2, 5, 5);
  }
  
  return true;
}
  

int CurveEditor2::value2pixel(int value) {
  int d = m_curve->get_max() - m_curve->get_min();
  int v = value - m_curve->get_min();
  return m_height - 2 - int(v / (double(d) / (m_height - 4)));
}


int CurveEditor2::pixel2value(int y) {
  int d = m_curve->get_max() - m_curve->get_min();
  return m_curve->get_max() - int((y - 2) * (double(d) / (m_height - 4)));
}


SongTime CurveEditor2::pixel2time(int x) {
  int64_t t = int64_t(x) * m_ticks_per_pixel;
  SongTime::Beat b = t >> (sizeof(SongTime::Tick) * 8);
  return SongTime(b, t - (int64_t(b) << (sizeof(SongTime::Tick) * 8)));
}


int CurveEditor2::time2pixel(const SongTime& time) {
  int64_t value = int64_t(time.get_beat()) * SongTime::ticks_per_beat() + 
    time.get_tick();
  return int(value / m_ticks_per_pixel);
}
