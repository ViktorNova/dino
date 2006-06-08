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
   along with this program; if not, write to the Free Software Foundation, 
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include <cassert>
#include <iostream>
#include <map>

#include "tracklabel.hpp"
#include "track.hpp"


using namespace std;
using namespace Dino;
using namespace Gdk;
using namespace Gtk;
using namespace Glib;
using namespace Pango;


TrackLabel::TrackLabel(const Song* song) 
  : m_song(song), 
    m_width(122), 
    m_height(20), 
    m_is_active(false),
    m_is_recording(false) {
  
  assert(song);
  m_colormap  = Colormap::get_system();
  m_bg_color.set_rgb(30000, 30000, 60000);
  m_fg_color.set_rgb(0, 0, 0);
  m_colormap->alloc_color(m_bg_color);
  m_colormap->alloc_color(m_fg_color);
  m_layout = Layout::create(get_pango_context());

  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  set_size_request(m_width, m_height + 4);
  
  m_kb_icon = Pixbuf::create_from_file(DATA_DIR "/tinykeyboard.png");
}
  

void TrackLabel::set_track(int id, Track* track) {
  assert(track);
  m_track = track;
  m_id = id;
  m_name_connection.disconnect();
  m_name_connection = m_track->signal_name_changed().
    connect(mem_fun(*this, &TrackLabel::slot_name_changed));
  slot_name_changed(track->get_name());
}


void TrackLabel::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color);
  FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  win->clear();
}


bool TrackLabel::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (m_is_active) {
    m_gc->set_foreground(m_bg_color);
    win->draw_rectangle(m_gc, true, 0, 4, m_width, m_height + 4);
  }
  
  if (m_is_recording)
    win->draw_pixbuf(m_gc, m_kb_icon, 0, 0, 2, 4 + m_height / 2 - 8, 18, 16, 
                     RGB_DITHER_NONE, 0, 0);
  
  m_gc->set_foreground(m_fg_color);
  int lHeight = m_layout->get_pixel_logical_extents().get_height();
  win->draw_layout(m_gc, 24, 4 + (m_height - lHeight)/2, m_layout);
  return true;
}


bool TrackLabel::on_button_press_event(GdkEventButton* event) {
  signal_clicked(event->button);
  return true;
}


bool TrackLabel::on_button_release_event(GdkEventButton* event) {
  return true;
}


bool TrackLabel::on_motion_notify_event(GdkEventMotion* event) {
  return true;
}


void TrackLabel::update() {
  RefPtr<Gdk::Window> win = get_window();
  if (win) {
    win->invalidate_rect(Gdk::Rectangle(0, 0, m_width, m_height + 4), false);
    win->process_updates(false);
  }
}


void TrackLabel::set_active_track(int id) {
  if (m_is_active != (id == m_id)) {
    m_is_active = (id == m_id);
    update();
  }
}


void TrackLabel::set_recording(bool recording) {
  m_is_recording = recording;
  queue_draw();
}


void TrackLabel::slot_name_changed(const string& name) {
  char tmp[10];
  sprintf(tmp, "%03d ", m_id);
  //m_layout->set_text(string(tmp));
  m_layout->set_text(string(tmp) + name);
  update();
}
