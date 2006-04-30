/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <larsl@users.sourceforge.net>
   
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
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include <cassert>
#include <iostream>
#include <map>

#include "debug.hpp"
#include "trackwidget.hpp"
#include "pattern.hpp"
#include "song.hpp"
#include "track.hpp"


using namespace std;
using namespace Dino;
using namespace Gdk;
using namespace Gtk;
using namespace Glib;
using namespace Pango;


TrackWidget::TrackWidget(const Song* song) 
  : m_song(song), m_col_width(20), m_drag_beat(-1), m_drag_pattern(-1),
    m_current_beat(0) {
  assert(song);
  m_colormap  = Colormap::get_system();
  m_bg_color.set_rgb(65535, 65535, 65535);
  m_bg_color2.set_rgb(65535, 60000, 50000);
  m_fg_color.set_rgb(40000, 40000, 65535);
  m_grid_color.set_rgb(40000, 40000, 40000);
  m_edge_color.set_rgb(0, 0, 0);
  m_hl_color.set_rgb(65535, 0, 0);
  m_colormap->alloc_color(m_bg_color);
  m_colormap->alloc_color(m_bg_color2);
  m_colormap->alloc_color(m_fg_color);
  m_colormap->alloc_color(m_grid_color);
  m_colormap->alloc_color(m_edge_color);
  m_colormap->alloc_color(m_hl_color);
  
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  set_size_request(m_col_width * m_song->get_length(), m_col_width + 4);
  
  song->signal_length_changed.
    connect(mem_fun(*this, &TrackWidget::slot_length_changed));
}
  

void TrackWidget::set_track(Track* track) {
  assert(track);
  m_track = track;
}


void TrackWidget::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color);
  win->clear();
}


bool TrackWidget::on_expose_event(GdkEventExpose* event) {

  RefPtr<Gdk::Window> win = get_window();
  win->clear();

  int width = m_col_width * m_song->get_length();
  int height = m_col_width;
  
  // draw current beat
  if (m_current_beat < m_song->get_length()) {
    Gdk::Rectangle bounds(0, 0, width + 1, 4);
    m_gc->set_clip_rectangle(bounds);
    m_gc->set_foreground(m_grid_color);
    win->draw_rectangle(m_gc, true, m_current_beat * m_col_width, 
			0, m_col_width + 1, 4);
  }
  
  Gdk::Rectangle bounds(0, 4, width + 1, height + 4);
  m_gc->set_clip_rectangle(bounds);
  
  // draw background
  int bpb = 4;
  for (int b = 0; b < m_song->get_length(); ++b) {
    if (b % (2*bpb) < bpb)
      m_gc->set_foreground(m_bg_color);
    else
      m_gc->set_foreground(m_bg_color2);
    win->draw_rectangle(m_gc, true, b * m_col_width, 4, m_col_width, height);
  }
  m_gc->set_foreground(m_grid_color);
  win->draw_line(m_gc, 0, 4, width, 4);
  win->draw_line(m_gc, 0, height-1 + 4, width, height-1 + 4);
  for (int c = 0; c < m_song->get_length() + 1; ++c) {
    win->draw_line(m_gc, c * m_col_width, 4, c * m_col_width, height + 4);
  }
  
  // draw patterns
  Pango::FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  char tmp[10];
  Track::SequenceIterator se;
  for (se = m_track->seq_begin(); se != m_track->seq_end(); ++se) {
    m_gc->set_clip_rectangle(bounds);
    m_gc->set_foreground(m_fg_color);
    int length = se->length;
    win->draw_rectangle(m_gc, true, se->start * m_col_width, 4, 
			length * m_col_width, height - 1);
    m_gc->set_foreground(m_edge_color);
    win->draw_rectangle(m_gc, false, se->start * m_col_width, 4,
			length * m_col_width, height - 1);
    Glib::RefPtr<Pango::Layout> l = Pango::Layout::create(get_pango_context());
    sprintf(tmp, "%03d", se->pattern_id);
    l->set_text(tmp);
    int lHeight = l->get_pixel_logical_extents().get_height();
    Gdk::Rectangle textBounds(se->start * m_col_width, 0, 
			      length * m_col_width, height - 1);
    m_gc->set_clip_rectangle(textBounds);
    win->draw_layout(m_gc, se->start * m_col_width + 2, 
		     4 + (height - lHeight)/2, l);
  }
  
  return true;
}


bool TrackWidget::on_button_press_event(GdkEventButton* event) {
  signal_clicked(event->button);
  
  int beat = int(event->x) / m_col_width;

  switch (event->button) {
  case 1: {
    char tmp[10];
    Track::ConstPatternIterator iter = m_track->pat_begin();
    m_pattern_menu.items().clear();
    for ( ; iter != m_track->pat_end(); ++iter) {
      sprintf(tmp, "%03d", iter->get_id());
      Menu_Helpers::MenuElem
	elem(tmp, bind(bind(mem_fun(*this, &TrackWidget::slot_insert_pattern), 
			    beat), iter->get_id()));
      m_pattern_menu.items().push_back(elem);
    }
    if (m_pattern_menu.items().size() == 0) {
      Menu_Helpers::MenuElem elem("No patterns");
      m_pattern_menu.items().push_back(elem);
    }
    m_pattern_menu.popup(event->button, event->time);
    return true;
  }
    
  case 2: {
    Track::SequenceIterator se = m_track->seq_find(beat);
    if (se != m_track->seq_end()) {
      m_drag_beat = se->start;
      m_drag_pattern = se->pattern_id;
      m_track->set_seq_entry_length(se, beat - se->start + 1);
      update();
    }
    return true;
  }
    
  case 3:
    m_track->remove_sequence_entry(m_track->seq_find(beat));
    update();
    return true;
  } 
  
  return false;
}


bool TrackWidget::on_button_release_event(GdkEventButton* event) {
  if (event->button == 2)
    m_drag_beat = -1;
  return true;
}


bool TrackWidget::on_motion_notify_event(GdkEventMotion* event) {
  int beat = int(event->x) / m_col_width;

  if ((event->state & GDK_BUTTON2_MASK) && m_drag_beat != -1 &&
      beat >= m_drag_beat && 
      beat - m_drag_beat + 1 <= 
      int(m_track->pat_find(m_drag_pattern)->get_length())) {
    m_track->set_sequence_entry(m_drag_beat, m_drag_pattern, 
				beat - m_drag_beat + 1);
    update();
    return true;
  }
  
  return false;
}


void TrackWidget::slot_insert_pattern(int pattern, int position) {
  int length = m_track->pat_find(pattern)->get_length();
  m_track->set_sequence_entry(position, pattern, length);
  update();
}


void TrackWidget::slot_length_changed(int length) {
  set_size_request(m_col_width * length, m_col_width + 4);
}


void TrackWidget::update() {
  RefPtr<Gdk::Window> win = get_window();
  if (win) {
    win->invalidate_rect(Gdk::Rectangle(0,0, get_width(), get_height()), false);
    win->process_updates(false);
  }
}


void TrackWidget::set_current_beat(int beat) {
  if (beat != m_current_beat) {
    m_current_beat = beat;
    update();
  }
}
