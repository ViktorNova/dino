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
#include <iostream>
#include <map>
#include <vector>

#include "commandproxy.hpp"
#include "song.hpp"
#include "tempowidget.hpp"
#include "plugininterface.hpp"


using namespace Dino;
using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace std;


TempoWidget::TempoWidget(CommandProxy& proxy, const Song* song) 
  : m_proxy(proxy),
    m_song(song), 
    m_col_width(16),
    m_height(20),
    m_drag_beat(-1), 
    m_active_tempo(0) {

  assert(song);
  
  // initialise colours
  m_colormap = Colormap::get_system();
  m_bg_color.set_rgb(65535, 62000, 65535);
  m_bg_color2.set_rgb(65535, 57000, 65535);
  m_fg_color.set_rgb(65535, 30000, 65535);
  m_grid_color.set_rgb(40000, 40000, 40000);
  m_edge_color.set_rgb(0, 0, 0);
  m_hl_color.set_rgb(65535, 0, 50000);
  m_colormap->alloc_color(m_bg_color);
  m_colormap->alloc_color(m_bg_color2);
  m_colormap->alloc_color(m_fg_color);
  m_colormap->alloc_color(m_grid_color);
  m_colormap->alloc_color(m_edge_color);
  m_colormap->alloc_color(m_hl_color);
  
  // connect signals
  song->signal_tempo_changed().
    connect(mem_fun(*this, &TempoWidget::queue_draw));
  song->signal_length_changed().
    connect(mem_fun(*this, &TempoWidget::length_changed));
  
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  set_size_request(m_col_width * m_song->get_length().get_beat(), m_height);
}
  

void TempoWidget::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color);
  win->clear();
}


bool TempoWidget::on_expose_event(GdkEventExpose* event) {

  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  
  int width = m_col_width * m_song->get_length().get_beat();
  int height = m_height;
  Rectangle bounds(0, 0, width + 1, height);
  m_gc->set_clip_rectangle(bounds);
  
  // draw background
  int bpb = 4;
  for (int b = 0; b < m_song->get_length().get_beat(); ++b) {
    if (b % (2*bpb) < bpb)
      m_gc->set_foreground(m_bg_color);
    else
      m_gc->set_foreground(m_bg_color2);
    win->draw_rectangle(m_gc, true, b * m_col_width, 0, m_col_width, height);
  }
  m_gc->set_foreground(m_grid_color);
  win->draw_line(m_gc, 0, 0, width, 0);
  win->draw_line(m_gc, 0, height-1, width, height-1);
  for (int c = 0; c < m_song->get_length().get_beat() + 1; ++c) {
    win->draw_line(m_gc, c * m_col_width, 0, c * m_col_width, height);
  }
  
  // draw tempo changes
  Pango::FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  char tmp[10];
  Song::TempoIterator iter;
  //const TempoMap::TempoChange* tempo = m_song->get_tempo_changes();
  //for ( ; tempo; tempo = tempo->next) {
  for (iter = m_song->tempo_begin(); iter != m_song->tempo_end(); ++iter) {
    if (&(*iter) == m_active_tempo)
      continue;
    Rectangle bounds(int(iter->get_beat() * m_col_width), 0, 
		     int(m_height * 1.5) + 1, height);
    m_gc->set_clip_rectangle(bounds);
    vector<Point> points;
    int x = int(iter->get_beat() * m_col_width);
    points.push_back(Point(x, 0));
    points.push_back(Point(x + height, 0));
    points.push_back(Point(x + 1.5 * height, height / 2));
    points.push_back(Point(x + 1.5 * height, height-1));
    points.push_back(Point(x, height-1));
    m_gc->set_foreground(m_fg_color);
    win->draw_polygon(m_gc, true, points);
    m_gc->set_foreground(m_edge_color);
    win->draw_polygon(m_gc, false, points); 
    RefPtr<Pango::Layout> l = Pango::Layout::create(get_pango_context());
    sprintf(tmp, "%d", iter->get_bpm());
    l->set_text(tmp);
    int lHeight = l->get_pixel_logical_extents().get_height();
    win->draw_layout(m_gc, int(iter->get_beat() * m_col_width + 2), 
		     (height - lHeight) / 2, l);
  }
  
  if (m_active_tempo) {
    const TempoMap::TempoChange* tempo = m_active_tempo;
    Rectangle bounds(int(tempo->get_beat() * m_col_width), 0, 
		     int(m_col_width * 1.5) + 1, height);
    m_gc->set_clip_rectangle(bounds);
    vector<Point> points;
    points.push_back(Point(int(tempo->get_beat() * m_col_width), 0));
    points.push_back(Point(int((tempo->get_beat() + 1) * m_col_width), 0));
    points.push_back(Point(int((tempo->get_beat() + 1.5) * m_col_width), 
			   height / 2));
    points.push_back(Point(int((tempo->get_beat() + 1.5) * m_col_width), 
			   height-1));
    points.push_back(Point(int(tempo->get_beat() * m_col_width), height-1));
    m_gc->set_foreground(m_hl_color);
    win->draw_polygon(m_gc, true, points);
    m_gc->set_foreground(m_edge_color);
    win->draw_polygon(m_gc, false, points); 
    RefPtr<Pango::Layout> l = Pango::Layout::create(get_pango_context());
    sprintf(tmp, "%.0f", float(m_editing_bpm));
    l->set_text(tmp);
    int lHeight = l->get_pixel_logical_extents().get_height();
    win->draw_layout(m_gc, int(tempo->get_beat() * m_col_width + 2), 
		     (height - lHeight) / 2, l);
  }
  
  return true;
}


bool TempoWidget::on_button_press_event(GdkEventButton* event) {
  unsigned int beat = (unsigned int)(event->x) / m_col_width;
  
  switch (event->button) {
  case 1: {
    if (event->state & GDK_CONTROL_MASK) {
      if (beat >= 0 && beat < unsigned(m_song->get_length().get_beat())) {
        double bpm = m_song->get_current_tempo(beat);
        Song::TempoIterator iter;
	m_proxy.add_tempo_change(beat, bpm, &iter);
        if (iter != m_song->tempo_end()) {
          m_active_tempo = &*iter;
          m_drag_start_y = int(event->y);
          m_editing_bpm = int(m_active_tempo->get_bpm());
          update();
        }
      }
    }
    return true;
  }
    
  case 2: {
    if (event->state & GDK_CONTROL_MASK) {
      Song::TempoIterator iter = m_song->tempo_find(beat);
      if (iter != m_song->tempo_end() && iter->get_beat() == beat) {
        m_active_tempo = &*iter;
        m_drag_start_y = int(event->y);
        m_editing_bpm = int(m_active_tempo->get_bpm());
        update();
      }
      else
        m_active_tempo = 0;
    }
    return true;
  }
    
  case 3:
    if (event->state & GDK_CONTROL_MASK) {
      Song::TempoIterator iter = m_song->tempo_find(beat);
      if (iter != m_song->tempo_end() && iter->get_beat() == beat)
	m_proxy.remove_tempo_change(beat);
    }
    else {
      m_menu.popup(event->button, event->time);
    }
    return true;
  } 
  
  return false;
}


bool TempoWidget::on_button_release_event(GdkEventButton* event) {
  if ((event->button == 2 || event->button == 1) && m_active_tempo) {
    m_proxy.add_tempo_change(m_active_tempo->get_beat(), m_editing_bpm);
    m_active_tempo = 0;
    update();
  }
  return true;
}


bool TempoWidget::on_motion_notify_event(GdkEventMotion* event) {
  if (m_active_tempo) {
    int new_bpm = int(m_active_tempo->get_bpm() + 
		      (m_drag_start_y - event->y) / 2);
    new_bpm = new_bpm < 1 ? 1 : new_bpm;
    if (new_bpm != m_editing_bpm) {
      m_editing_bpm = new_bpm;
      update();
    }
  }
  return true;
}


void TempoWidget::update() {
  RefPtr<Gdk::Window> win = get_window();
  win->invalidate_rect(Rectangle(0, 0, get_width(), get_height()), false);
  win->process_updates(false);
}


void TempoWidget::length_changed(const SongTime& length) {
  set_size_request(m_col_width * length.get_beat(), m_height);
}


void TempoWidget::update_menu(PluginInterface& plif) {
  using namespace Menu_Helpers;
  m_menu.items().clear();
  PluginInterface::action_iterator iter;
  SongAction* sa;
  for (iter = plif.actions_begin(); iter != plif.actions_end(); ++iter) {
    if ((sa = dynamic_cast<SongAction*>(*iter))) {
      slot<void> aslot = bind(mem_fun(*sa, &SongAction::run), ref(m_proxy));
      m_menu.items().push_back(MenuElem((*iter)->get_name(), aslot));
    }
  }
}

