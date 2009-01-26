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

#include "commandproxy.hpp"
#include "noteeditor2.hpp"


using namespace Dino;
using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace Pango;
using namespace sigc;
using namespace std;


NoteEditor2::NoteEditor2(CommandProxy& proxy)
  : m_track(0),
    m_pattern(0),
    m_proxy(proxy),
    m_row_height(8),
    m_ticks_per_pixel(SongTime::ticks_per_beat() / 64),
    m_rows(0),
    m_snap(4),
    m_note_length(1, 0) {

  // initialise colours
  m_colormap = Colormap::get_system();
  m_bg_color.set_rgb(65535, 65535, 65535);
  m_bg_color2.set_rgb(60000, 60000, 65535);
  m_fg_color1.set_rgb(0, 0, 65535);
  m_grid_color.set_rgb(40000, 40000, 40000);
  m_edge_color.set_rgb(0, 0, 0);
  m_bad_hl_color.set_rgb(65535, 0, 0);
  m_good_hl_color.set_rgb(0, 65535, 0);
  m_selbox_color.set_rgb(20000, 20000, 40000);
  
  // initialise note colours for different velocities
  gushort red1 = 40000, green1 = 45000, blue1 = 40000;
  gushort red2 = 0, green2 = 30000, blue2 = 0;
  gushort red3 = 50000, green3 = 40000, blue3 = 40000;
  gushort red4 = 60000, green4 = 0, blue4 = 0;
  for (int i = 0; i < 16; ++i) {
    m_note_colors[i].set_rgb(gushort((red1 * (15 - i) + red2 * i) / 15.0),
			     gushort((green1 * (15 - i) + green2 * i) / 15.0),
			     gushort((blue1 * (15 - i) + blue2 * i) / 15.0));
    m_selected_note_colors[i].
      set_rgb(gushort((red3 * (15 - i) + red4 * i) / 15.0),
	      gushort((green3 * (15 - i) + green4 * i) / 15.0),
	      gushort((blue3 * (15 - i) + blue4 * i) / 15.0));
  }
  
  // allocate all colours
  m_colormap->alloc_color(m_bg_color);
  m_colormap->alloc_color(m_bg_color2);
  m_colormap->alloc_color(m_fg_color1);
  m_colormap->alloc_color(m_grid_color);
  m_colormap->alloc_color(m_edge_color);
  m_colormap->alloc_color(m_bad_hl_color);
  m_colormap->alloc_color(m_good_hl_color);
  m_colormap->alloc_color(m_selbox_color);
  for (int i = 0; i < 16; ++i) {
    m_colormap->alloc_color(m_note_colors[i]);
    m_colormap->alloc_color(m_selected_note_colors[i]);
  }

  m_layout = Pango::Layout::create(get_pango_context());
  m_layout->set_text("127");
  
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | 
	     BUTTON_MOTION_MASK | SCROLL_MASK | POINTER_MOTION_MASK);
}

  
void NoteEditor2::set_pattern(const Track& track, const Pattern& pattern) {
  m_track = &track;
  m_pattern = &pattern;
  m_rows = (m_track->get_mode() == Track::DrumMode ? 
	    m_track->get_keys().size() : 128);
  m_selection = NoteSelection(&pattern);
  
  // reset connections
  m_note_added_conn.disconnect();
  m_note_removed_conn.disconnect();
  m_note_changed_conn.disconnect();
  slot<void> draw = mem_fun(*this, &NoteEditor2::queue_draw);
  m_note_added_conn = m_pattern->signal_note_added().
    connect(sigc::hide(draw));
  m_note_removed_conn = m_pattern->signal_note_removed().
    connect(sigc::hide(draw));
  m_note_changed_conn = m_pattern->signal_note_changed().
    connect(sigc::hide(draw));
  
  set_size_request(time2pixel(m_pattern->get_length()) + 1, 
		   m_rows * m_row_height + 1);
  draw();
}


void NoteEditor2::unset_pattern() {
  m_track = 0;
  m_pattern = 0;
  m_selection = NoteSelection();

  m_note_added_conn.disconnect();
  m_note_removed_conn.disconnect();
  m_note_changed_conn.disconnect();

  set_size_request(0, 0);
  queue_draw();
}


void NoteEditor2::set_vadjustment(Gtk::Adjustment* adj) {
  m_vadj = adj;
}


void NoteEditor2::cut_selection() {

}


void NoteEditor2::copy_selection() {

}


void NoteEditor2::paste() {

}


void NoteEditor2::delete_selection() {

}


void NoteEditor2::select_all() {

}


void NoteEditor2::update_menu(PluginInterface& plif) {

}
 

bool NoteEditor2::on_button_press_event(GdkEventButton* event) {
  
  if (!m_pattern)
    return true;
  
  SongTime time = pixel2time(event->x);
  unsigned char key = pixel2key(event->y);
  
  cerr<<"time = "<<time.get_beat()<<":"<<time.get_tick()
      <<", key = "<<int(key)<<endl;
  
  // if we're not on the editor surface, abort
  if (time < SongTime(0, 0) || time >= m_pattern->get_length() || key > 127)
    return true;
  
  if (event->button == 1) {
    
    // button 1 + ctrl - add a new note
    if (event->state & GDK_CONTROL_MASK)
      start_adding_note(time, key);
    
    // button 1 + shift - add to or remove from selection
    else if (event->state & GDK_SHIFT_MASK)
      start_selecting(time, key, false);
    
    // button 1 without modifiers - start a new selection
    else
      start_selecting(time, key, true);
  }
  
  return true;
}


bool NoteEditor2::on_button_release_event(GdkEventButton* event) {

  if (!m_pattern)
    return true;
  
  SongTime time = snap(pixel2time(event->x));
  unsigned char key = pixel2key(event->y);
  
  if (m_drag_operation != DragNoOperation) {
    end_drag(time > m_drag_max_time ? m_drag_max_time : time, key);
  }
  
  return true;
}


bool NoteEditor2::on_motion_notify_event(GdkEventMotion* event) {

  if (!m_pattern)
    return true;
  
  if (m_drag_operation == DragNoOperation)
    return true;
  
  SongTime time = snap(pixel2time(event->x));
  unsigned char key = pixel2key(event->y);
  
  time = time > m_drag_max_time ? m_drag_max_time : time;
  
  if (time != m_drag_time || key != m_drag_key) {
    m_drag_time = time;
    m_drag_key = key;
    queue_draw();
  }
  
  return true;
}


bool NoteEditor2::on_expose_event(GdkEventExpose* event) {
  
  cerr<<__PRETTY_FUNCTION__<<endl;
  
  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (!m_pattern)
    return true;
  
  draw_background(win);
  
  Pattern::NoteIterator iter;
  for (iter = m_pattern->notes_begin(); iter != m_pattern->notes_end(); ++iter)
    draw_note(win, iter);
  
  if (m_drag_operation == DragResizeNotes) {
    cerr<<"*** DRAW OUTLINES ***"<<endl;
    if (m_drag_time > m_drag_start_time) {
      SongTime length = m_drag_time - m_drag_start_time;
      NoteSelection::Iterator iter;
      for (iter = m_selection.begin(); iter != m_selection.end(); ++iter)
	draw_outline(win, iter->get_time(), iter->get_key(), length, true);
    }
  }

  
  return true;
}


bool NoteEditor2::on_scroll_event(GdkEventScroll* event) {
  
  return true;
}


void NoteEditor2::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color1);
  FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  win->clear();
}


void NoteEditor2::draw_background(Glib::RefPtr<Gdk::Window>& win) {

  // draw the background and the vertical grid
  bool bg = true;
  int beat_width = time2pixel(SongTime(1, 0));
  SongTime::Beat b;
  for (b = 0; b < m_pattern->get_length().get_beat(); ++b) {
    int x = time2pixel(SongTime(b, 0));
    m_gc->set_foreground( bg ? m_bg_color : m_bg_color2);
    bg = !bg;
    win->draw_rectangle(m_gc, true, x, 0, beat_width, m_rows * m_row_height);
    m_gc->set_foreground(m_grid_color);
    win->draw_line(m_gc, x, 0, x, m_rows * m_row_height);
  }
  SongTime::Tick t = m_pattern->get_length().get_tick();
  if (t > 0) {
    m_gc->set_foreground( bg ? m_bg_color : m_bg_color2);
    win->draw_rectangle(m_gc, true, time2pixel(SongTime(b, 0)),
			0, time2pixel(SongTime(0, t)), m_rows * m_row_height);
  }
  int x = time2pixel(m_pattern->get_length());
  m_gc->set_foreground(m_grid_color);
  win->draw_line(m_gc, x, 0, x, m_rows * m_row_height);
  
  // draw the horizontal grid
  for (unsigned i = 0; i < m_rows + 1; ++i) {
    int y = i * m_row_height;
    win->draw_line(m_gc, 0, y, x, y);
  }
}


void NoteEditor2::draw_note(Glib::RefPtr<Gdk::Window>& win, 
			    const Pattern::NoteIterator& iter) {

  int x = time2pixel(iter->get_time());
  int l = time2pixel(iter->get_time() + iter->get_length()) - x;

  m_gc->set_foreground(m_note_colors[int(iter->get_velocity() / 8)]);
  win->draw_rectangle(m_gc, true, x, key2pixel(iter->get_key()), 
		      l, m_row_height);
  m_gc->set_foreground(m_edge_color);
  win->draw_rectangle(m_gc, false, x, key2pixel(iter->get_key()), 
		      l, m_row_height);
}


void NoteEditor2::draw_outline(Glib::RefPtr<Gdk::Window>& win,
			       const SongTime& start, unsigned char key,
			       const SongTime& length, bool good) {
  
  int x = time2pixel(start);
  int l = time2pixel(start + length) - x;

  m_gc->set_foreground(good ? m_good_hl_color : m_bad_hl_color);
  win->draw_rectangle(m_gc, false, x, key2pixel(key), l, m_row_height);
}


void NoteEditor2::end_drag(const SongTime& time, unsigned char key) {
  
  if (m_drag_operation == DragResizeNotes) {
    if (time > m_drag_start_time) {
      m_note_length = time - m_drag_start_time;
      NoteSelection::Iterator iter;
      for (iter = m_selection.begin(); iter != m_selection.end(); ++iter) {
	m_proxy.set_note_size(m_track->get_id(), m_pattern->get_id(),
			      m_drag_start_time, m_drag_start_key,
			      m_note_length);
      }
    }
  }
  
  m_drag_operation = DragNoOperation;
  queue_draw();
}


int NoteEditor2::key2pixel(unsigned char key) {
  return (127 - key) * m_row_height;
}


unsigned char NoteEditor2::pixel2key(int y) {
  return 127 - (y / m_row_height);
}


SongTime NoteEditor2::pixel2time(int x) {
  int64_t t = int64_t(x) * m_ticks_per_pixel;
  SongTime::Beat b = t >> (sizeof(SongTime::Tick) * 8);
  return SongTime(b, t - (int64_t(b) << (sizeof(SongTime::Tick) * 8)));
}


SongTime NoteEditor2::snap(const SongTime& time) {
  if (m_snap == 0)
    return time;
  double step = SongTime::ticks_per_beat() / double(m_snap);
  SongTime t = time + SongTime(0, SongTime::Tick(step / 2));
  SongTime::Tick n = t.get_tick() / step;
  return SongTime(t.get_beat(), n * step);
}


void NoteEditor2::start_adding_note(const SongTime& time, unsigned char key) {
  m_selection.clear();
  
  SongTime t = snap(time);
  
  // check if there already is a note here, in that case shorten or remove it
  Pattern::NoteIterator old_note = m_pattern->find_note(t, key);
  if (old_note != m_pattern->notes_end()) {
    
    // if it starts at the same time as the new one, remove it
    if (old_note->get_time() == t) {
      if (!m_proxy.delete_note(m_track->get_id(), m_pattern->get_id(),
			       old_note->get_time(), key))
	return;
    }
    
    // otherwise, shorten it
    else {
      if (!m_proxy.set_note_size(m_track->get_id(), m_pattern->get_id(),
				 old_note->get_time(), key, 
				 t - old_note->get_time()))
	return;
    }
  }
  
  SongTime max = m_pattern->check_maximal_free_space(t, key, 
						     m_pattern->get_length());
  SongTime length = m_note_length > max ? max : m_note_length;
  
  if (m_proxy.add_note(m_track->get_id(), m_pattern->get_id(), 
		       t, key, 64, length)) {
    m_note_length = length;
    Pattern::NoteIterator iter = m_pattern->find_note(t, key);
    m_selection.add_note(iter);
    m_drag_operation = DragResizeNotes;
    m_drag_start_time = t;
    m_drag_start_key = key;
    m_drag_time = t + length;
    m_drag_key = key;
    m_drag_max_time = t + max;
  }
}


void NoteEditor2::start_selecting(const SongTime& time, unsigned char key,
				  bool clear) {

}


int NoteEditor2::time2pixel(const SongTime& time) {
  int64_t value = int64_t(time.get_beat()) * SongTime::ticks_per_beat() + 
    time.get_tick();
  return int(value / m_ticks_per_pixel);
}
