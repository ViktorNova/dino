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

#include <iostream>

#include "pattern.hpp"
#include "ruler.hpp"
#include "song.hpp"
#include "track.hpp"


using namespace std;
using namespace Gdk;
using namespace Dino;


Ruler::Ruler(const SongTime& length, int subs, int interval, 
	     SongTime::Tick ticks_per_pixel, int height)
  : m_length(length), 
    m_subs(subs), 
    m_interval(interval), 
    m_ticks_per_pixel(ticks_per_pixel), 
    m_height(height),
    m_loop_start(-1, 0),
    m_loop_end(-1, 0) {
  
  m_fg.set_rgb(0, 0, 0);
  m_loop_bg.set_rgb(65000, 65000, 40000);
  m_loop_marker.set_rgb(60000, 40000, 0);
  m_colormap = Colormap::get_system();
  m_colormap->alloc_color(m_fg);
  m_colormap->alloc_color(m_loop_bg);
  m_colormap->alloc_color(m_loop_marker);
  
  set_size_request(time2pixel(m_length) + 1, m_height);
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
}

  
void Ruler::set_length(const SongTime& length) {
  m_length = length;
  set_size_request(time2pixel(m_length) + 1, m_height);
}


void Ruler::set_subdivisions(int subs) {
  m_subs = subs;
}


void Ruler::set_interval(int interval) {
  m_interval = interval;
}


void Ruler::set_ticks_per_pixel(SongTime::Tick ticks_per_pixel) {
  m_ticks_per_pixel = ticks_per_pixel;
  set_size_request(time2pixel(m_length) + 1, m_height);
}


void Ruler::on_realize() {
  Gtk::DrawingArea::on_realize();
  Glib::RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  win->clear();
}


bool Ruler::on_expose_event(GdkEventExpose* event) {

  Glib::RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (m_length <= SongTime(0, 0))
    return true;
  
  Pango::FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  char tmp[10];
  
  int start = time2pixel(m_loop_start);
  int end = time2pixel(m_loop_end);
  
  // if we are looping over an interval, draw a yellow background
  if (m_loop_start >= SongTime(0, 0) && m_loop_start < m_loop_end) {
    m_gc->set_foreground(m_loop_bg);
    win->draw_rectangle(m_gc, true, start, 0, end - start, m_height);
  }
  
  // draw the loop start marker
  m_gc->set_foreground(m_loop_marker);
  if (m_loop_start >= SongTime(0, 0)) {
    win->draw_rectangle(m_gc, true, start, 0, 3, m_height);
    win->draw_rectangle(m_gc, true, start + 4, m_height / 3 - 1, 3, 3);
    win->draw_rectangle(m_gc, true, start + 4, 2 * m_height / 3 - 1, 3, 3);
  }
  
  // and the loop end marker
  if (m_loop_end >= SongTime(0, 0)) {
    win->draw_rectangle(m_gc, true, end - 2, 0, 3, m_height);
    win->draw_rectangle(m_gc, true, end - 6, m_height / 3 - 1, 3, 3);
    win->draw_rectangle(m_gc, true, end - 6, 2 * m_height / 3 - 1, 3, 3);
  }
  
  // draw the ticks and numbers
  m_gc->set_foreground(m_fg);
  for (int i = 0; i <= m_length.get_beat(); ++i) {
    int x = time2pixel(SongTime(i, 0));
    win->draw_line(m_gc, x, m_height - 4, x, m_height);
    if (i % m_interval == 0 && i != 0) {
      Glib::RefPtr<Pango::Layout> l = 
	Pango::Layout::create(get_pango_context());
      sprintf(tmp, "%d", i);
      l->set_text(tmp);
      Pango::Rectangle rect = l->get_pixel_logical_extents();
      win->draw_layout(m_gc, x - rect.get_width() / 2, 
		       (m_height - 4 - rect.get_height()) / 2, l);
    }
    for (int j = 1; j < m_subs; ++j) {
      int b = time2pixel(SongTime(1, 0));
      win->draw_line(m_gc, x + j * b / m_subs, 
		     m_height- 2, x + j * b / m_subs, m_height);
    }
  }
  
  return true;
}


bool Ruler::on_button_press_event(GdkEventButton* event) {
  signal_clicked(pixel2time(event->x), event->button);
  return true;
}


void Ruler::set_loop_start(const SongTime& start) {
  m_loop_start = start;
  queue_draw();
}


void Ruler::set_loop_end(const SongTime& end) {
  m_loop_end = end;
  queue_draw();
}


SongTime Ruler::pixel2time(int x) {
  int64_t t = int64_t(x) * m_ticks_per_pixel;
  SongTime::Beat b = t >> (sizeof(SongTime::Tick) * 8);
  return SongTime(b, t - (int64_t(b) << (sizeof(SongTime::Tick) * 8)));
}


int Ruler::time2pixel(const SongTime& time) {
  int64_t value = int64_t(time.get_beat()) * SongTime::ticks_per_beat() + 
    time.get_tick();
  return int(value / m_ticks_per_pixel);
}


PatternRuler::PatternRuler()
  : Ruler(SongTime(0, 0), 4, 1, SongTime::ticks_per_beat() / 64, 20),
    m_song(0) {

}


void PatternRuler::set_song(const Song* song) {
  m_song = song;
}


void PatternRuler::set_pattern(int track, int pattern) {
  if (track != -1 && pattern != -1) {
    // XXX need to connect to the length and steps signals
    const Pattern* pat = &*m_song->tracks_find(track)->pat_find(pattern);
    set_length(pat->get_length());
    set_interval(1);
  }
  else
    set_length(SongTime(0, 0));
}
