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

#include "commandproxy.hpp"
#include "debug.hpp"
#include "sequencewidget.hpp"
#include "pattern.hpp"
#include "plugininterface.hpp"
#include "song.hpp"
#include "track.hpp"


using namespace Dino;
using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace Pango;
using namespace sigc;
using namespace std;


SequenceWidget::SequenceWidget(CommandProxy& proxy) 
  : m_proxy(proxy),
    m_track(0),
    m_col_width(20), 
    //m_drag_beat(-1), 
    //m_drag_pattern(-1),
    m_drag_seqid(-1),
    m_current_beat(0) {
  
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
  set_size_request(0, m_col_width + 4);
  
}
  

void SequenceWidget::set_track(const Track* track) {
  assert(track);
  m_track = track;
  // XXX Need to disconnect the old track here
  m_track->signal_length_changed().
    connect(mem_fun(*this, &SequenceWidget::slot_length_changed));
  slot<void> draw = mem_fun(*this, &SequenceWidget::queue_draw);
  m_track->signal_sequence_entry_added().
    connect(sigc::hide(sigc::hide(sigc::hide(draw))));
  m_track->signal_sequence_entry_changed().
    connect(sigc::hide(sigc::hide(sigc::hide(draw))));
  set_size_request(m_col_width * m_track->get_length(), m_col_width + 4);
}


void SequenceWidget::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color);
  win->clear();
}


bool SequenceWidget::on_expose_event(GdkEventExpose* event) {
  
  if (!m_track)
    return true;
  
  RefPtr<Gdk::Window> win = get_window();
  win->clear();

  int width = m_col_width * m_track->get_length();
  int height = m_col_width;
  
  // draw current beat
  if (m_current_beat < m_track->get_length()) {
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
  for (int b = 0; b < m_track->get_length(); ++b) {
    if (b % (2*bpb) < bpb)
      m_gc->set_foreground(m_bg_color);
    else
      m_gc->set_foreground(m_bg_color2);
    win->draw_rectangle(m_gc, true, b * m_col_width, 4, m_col_width, height);
  }
  m_gc->set_foreground(m_grid_color);
  win->draw_line(m_gc, 0, 4, width, 4);
  win->draw_line(m_gc, 0, height-1 + 4, width, height-1 + 4);
  for (int c = 0; c < m_track->get_length() + 1; ++c) {
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
    int length = se->get_length();
    win->draw_rectangle(m_gc, true, se->get_start() * m_col_width, 4, 
			length * m_col_width, height - 1);
    m_gc->set_foreground(m_edge_color);
    win->draw_rectangle(m_gc, false, se->get_start() * m_col_width, 4,
			length * m_col_width, height - 1);
    Glib::RefPtr<Pango::Layout> l = Pango::Layout::create(get_pango_context());
    sprintf(tmp, "%03d", se->get_pattern_id());
    l->set_text(tmp);
    int lHeight = l->get_pixel_logical_extents().get_height();
    Gdk::Rectangle textBounds(se->get_start() * m_col_width, 0, 
			      length * m_col_width, height - 1);
    m_gc->set_clip_rectangle(textBounds);
    win->draw_layout(m_gc, se->get_start() * m_col_width + 2, 
		     4 + (height - lHeight)/2, l);
  }
  
  return true;
}


bool SequenceWidget::on_button_press_event(GdkEventButton* event) {
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
	elem(tmp, bind(bind(mem_fun(*this, &SequenceWidget::slot_insert_pattern), 
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
      //m_drag_beat = se->get_start();
      //m_drag_pattern = se->get_pattern_id();
      m_drag_seqid = se->get_id();
      m_proxy.set_sequence_entry_length(m_track->get_id(), beat,
					beat - se->get_start() + 1);
    }
    return true;
  }
    
  case 3:
    if (event->state & GDK_CONTROL_MASK)
      m_proxy.remove_sequence_entry(m_track->get_id(), beat);
    else
      m_action_menu.popup(event->button, event->time);
    return true;
  } 
  
  return false;
}


bool SequenceWidget::on_button_release_event(GdkEventButton* event) {
  if (event->button == 2)
    m_drag_seqid = -1;
  return true;
}


bool SequenceWidget::on_motion_notify_event(GdkEventMotion* event) {
  int beat = int(event->x) / m_col_width;
  
  if ((event->state & GDK_BUTTON2_MASK) && m_drag_seqid != -1) {
    Track::SequenceIterator siter = m_track->seq_find_by_id(m_drag_seqid);
    if (siter != m_track->seq_end() && beat >= siter->get_start() &&
        beat - siter->get_start() + 1 <= siter->get_pattern().get_length()) {
      m_proxy.set_sequence_entry_length(m_track->get_id(), siter->get_start(),
					beat - siter->get_start() + 1);
      return true;
    }
  }
  
  return false;
}


void SequenceWidget::slot_insert_pattern(int pattern, int position) {
  int length = m_track->pat_find(pattern)->get_length();
  m_proxy.add_sequence_entry(m_track->get_id(), position, pattern, length);
  //m_track->set_sequence_entry(position, pattern, length);
  update();
}


void SequenceWidget::slot_length_changed(int length) {
  set_size_request(m_col_width * length, m_col_width + 4);
}


void SequenceWidget::update() {
  RefPtr<Gdk::Window> win = get_window();
  if (win) {
    win->invalidate_rect(Gdk::Rectangle(0,0, get_width(), get_height()), false);
    win->process_updates(false);
  }
}


void SequenceWidget::set_current_beat(int beat) {
  if (beat != m_current_beat) {
    m_current_beat = beat;
    update();
  }
}


void SequenceWidget::update_menu(PluginInterface& plif) {
  using namespace Menu_Helpers;
  m_action_menu.items().clear();
  PluginInterface::action_iterator iter;
  TrackAction* ta;
  for (iter = plif.actions_begin(); iter != plif.actions_end(); ++iter) {
    if ((ta = dynamic_cast<TrackAction*>(*iter))) {
      slot<void> aslot = bind(mem_fun(*ta, &TrackAction::run), 
			      ref(m_proxy), 
			      ref(m_proxy.get_song().
				  tracks_find(m_track->get_id())));
      m_action_menu.items().push_back(MenuElem((*iter)->get_name(), aslot));
    }
  }
}

