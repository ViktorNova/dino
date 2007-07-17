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


Ruler::Ruler(int length, int subs, int interval, int size, int height)
  : m_length(length), 
    m_subs(subs), 
    m_interval(interval), 
    m_div_size(size), 
    m_height(height),
    m_loop_start(-1),
    m_loop_end(-1) {
  
  m_fg.set_rgb(0, 0, 0);
  m_loop_bg.set_rgb(65000, 65000, 40000);
  m_loop_marker.set_rgb(60000, 40000, 0);
  m_colormap = Colormap::get_system();
  m_colormap->alloc_color(m_fg);
  m_colormap->alloc_color(m_loop_bg);
  m_colormap->alloc_color(m_loop_marker);
  
  set_size_request(m_length * m_div_size + 1, m_height);
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
}

  
void Ruler::set_length(int length) {
  m_length = length;
  set_size_request(m_length * m_div_size + 1, m_height);
}


void Ruler::set_subdivisions(int subs) {
  m_subs = subs;
}


void Ruler::set_interval(int interval) {
  m_interval = interval;
}


void Ruler::set_division_size(int size) {
  m_div_size = size;
  set_size_request(m_length * m_div_size + 1, m_height);
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
  if (m_length <= 0)
    return true;
  
  Pango::FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  char tmp[10];
  
  // if we are looping over an interval, draw a yellow background
  if (m_loop_start != -1 && m_loop_start < m_loop_end) {
    m_gc->set_foreground(m_loop_bg);
    win->draw_rectangle(m_gc, true, m_loop_start * m_div_size, 0,
			(m_loop_end - m_loop_start) * m_div_size, m_height);
  }
  
  // draw the loop start marker
  m_gc->set_foreground(m_loop_marker);
  if (m_loop_start != -1) {
    win->draw_rectangle(m_gc, true, 
			m_loop_start * m_div_size, 0, 3, m_height);
    win->draw_rectangle(m_gc, true, m_loop_start * m_div_size + 4,
			m_height / 3 - 1, 3, 3);
    win->draw_rectangle(m_gc, true, m_loop_start * m_div_size + 4,
			2 * m_height / 3 - 1, 3, 3);
  }
  
  // and the loop end marker
  if (m_loop_end != -1) {
    win->draw_rectangle(m_gc, true, 
			m_loop_end * m_div_size - 2, 0, 3, m_height);
    win->draw_rectangle(m_gc, true, m_loop_end * m_div_size - 6,
			m_height / 3 - 1, 3, 3);
    win->draw_rectangle(m_gc, true, m_loop_end * m_div_size - 6,
			2 * m_height / 3 - 1, 3, 3);
  }
  
  // draw the ticks and numbers
  m_gc->set_foreground(m_fg);
  for (int i = 0; i <= m_length; ++i) {
    win->draw_line(m_gc, i * m_div_size, m_height - 4, 
		   i * m_div_size, m_height);
    if (i % m_interval == 0 && i != 0) {
      Glib::RefPtr<Pango::Layout> l = 
	Pango::Layout::create(get_pango_context());
      sprintf(tmp, "%d", i);
      l->set_text(tmp);
      Pango::Rectangle rect = l->get_pixel_logical_extents();
      win->draw_layout(m_gc, i * m_div_size - rect.get_width() / 2, 
		       (m_height - 4 - rect.get_height()) / 2, l);
    }
    if (i == m_length)
      break;
    for (int j = 1; j < m_subs; ++j) {
      win->draw_line(m_gc, i * m_div_size + j * m_div_size / m_subs, 
		     m_height- 2, i * m_div_size + j * m_div_size / m_subs, 
		     m_height);
    }
  }
  
  return true;
}


bool Ruler::on_button_press_event(GdkEventButton* event) {
  double pos = event->x / m_div_size;
  signal_clicked(pos, event->button);
  return true;
}


void Ruler::set_loop_start(int start) {
  m_loop_start = start;
  queue_draw();
}


void Ruler::set_loop_end(int end) {
  m_loop_end = end;
  queue_draw();
}


PatternRuler::PatternRuler()
  : Ruler(0, 1, 1, 20, 20),
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
    set_subdivisions(pat->get_steps());
    set_interval(1);
    set_division_size(8 * pat->get_steps());
  }
  else
    set_length(0);
}
