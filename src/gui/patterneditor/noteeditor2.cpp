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
    m_proxy(proxy) {

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
  set_size_request(time2pixel(m_pattern->get_length()) + 1, 
		   m_rows * m_row_height + 1);

}


void NoteEditor2::unset_pattern() {
  m_track = 0;
  m_pattern = 0 ;
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
  return true;
}


bool NoteEditor2::on_button_release_event(GdkEventButton* event) {
  return true;
}


bool NoteEditor2::on_motion_notify_event(GdkEventMotion* event) {
  return true;
}


bool NoteEditor2::on_expose_event(GdkEventExpose* event) {
  
  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (!m_pattern)
    return true;
  
  draw_background(win);
  
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
  bool bg = true;
  int beat_width = time2pixel(SongTime(1, 0));
  SongTime::Beat b;
  for (b = 0; b < m_pattern->get_length().get_beat(); ++b) {
    if (bg)
      m_gc->set_foreground(m_bg_color);
    else
      m_gc->set_foreground(m_bg_color2);
    win->draw_rectangle(m_gc, true, time2pixel(SongTime(b, 0)),
			0, beat_width, 128 * m_row_height);
    bg = !bg;
  }
  SongTime::Tick t = m_pattern->get_length().get_tick();
  if (t > 0) {
    win->draw_rectangle(m_gc, true, time2pixel(SongTime(b, 0)),
			0, time2pixel(SongTime(0, t)), 128 * m_row_height);
  }
}


//int pixel2row(int y);
//SongTime pixel2time(int x);
//int row2pixel(int row);
int NoteEditor2::time2pixel(const SongTime& time) {
  int64_t value = time.get_beat() * SongTime::ticks_per_beat() + 
    time.get_tick();
  return int(m_time_scale * (value / double(SongTime::ticks_per_beat())));
}


//int row2key(int row);
//int key2row(int row);
  
