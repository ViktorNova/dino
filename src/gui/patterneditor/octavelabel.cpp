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

#include "debug.hpp"
#include "keyinfo.hpp"
#include "octavelabel.hpp"


using namespace std;
using namespace Gtk;
using namespace Gdk;


OctaveLabel::OctaveLabel(int width, int note_height)
  : m_width(width), 
    m_note_height(note_height),
    m_drum_height(20) {
  m_bg_color.set_rgb(60000, 60000, 65535);
  m_fg_color.set_rgb(40000, 40000, 40000);
  m_colormap = Colormap::get_system();
  m_colormap->alloc_color(m_bg_color);
  m_colormap->alloc_color(m_fg_color);
  set_size_request(m_width, 128 * m_note_height + 1);
}

  
void OctaveLabel::on_realize() {
  Gtk::DrawingArea::on_realize();
  Glib::RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  win->clear();
}


bool OctaveLabel::on_expose_event(GdkEventExpose* event) {
  
  Dino::Track::Mode mode = 
    m_track ? m_track->get_mode() : Dino::Track::DrumMode;

  Glib::RefPtr<Gdk::Window> win = get_window();
  win->clear();
  m_gc->set_foreground(m_bg_color);
  Pango::FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  char tmp[100];
  Glib::RefPtr<Pango::Layout> l = Pango::Layout::create(get_pango_context());

  if (mode == Dino::Track::DrumMode) {
    const vector<Dino::KeyInfo*>& keys = m_track->get_keys();
    win->draw_rectangle(m_gc, true, 0, 0, get_width(), 
			keys.size() * m_drum_height);
    m_gc->set_foreground(m_fg_color);
    for (unsigned i = 0; i < keys.size(); ++i) {
      win->draw_line(m_gc, 0, m_drum_height * (keys.size() - i),
		     get_width(), m_drum_height * (keys.size() - i));
      l->set_text(keys[i]->get_name());
      Pango::Rectangle rect = l->get_pixel_logical_extents();
      win->draw_layout(m_gc, 4, 
		       m_drum_height * (keys.size() - i - 1) + 
		       (m_drum_height - rect.get_height()) / 2, l);
    }
  }
  
  else {
    win->draw_rectangle(m_gc, true, 0, 0, get_width(), 128 * m_note_height);
    m_gc->set_foreground(m_fg_color);
    for (int i = 0; i < 11; ++i) {
      win->draw_line(m_gc, 0, m_note_height * (128 - 12 * i), 
		     get_width(), m_note_height * (128 - 12 * i));
      sprintf(tmp, "%d", i);
      l->set_text(tmp);
      Pango::Rectangle rect = l->get_pixel_logical_extents();
      win->draw_layout(m_gc, (get_width() - rect.get_width()) / 2,
		       (128 - 12*i - 6) * m_note_height - rect.get_height() / 2,
		       l);
    }
  }
  
  return true;
}


void OctaveLabel::set_track(const Dino::Track* track) {
  if (m_track != track) {
    m_track = track;
    m_mode_conn.disconnect();
    m_add_conn.disconnect();
    m_remove_conn.disconnect();
    m_change_conn.disconnect();
    m_move_conn.disconnect();
    if (m_track) {
      m_mode_conn = m_track->signal_mode_changed().
	connect(mem_fun(*this, &OctaveLabel::track_mode_changed));
      sigc::slot<void> update = 
	sigc::compose(sigc::mem_fun(*this, &OctaveLabel::track_mode_changed),
		      sigc::mem_fun(*track, &Dino::Track::get_mode));
      m_add_conn = m_track->signal_key_added().connect(sigc::hide(update));
      m_remove_conn = m_track->signal_key_removed().connect(sigc::hide(update));
      m_change_conn = m_track->signal_key_changed().connect(sigc::hide(update));
      m_move_conn = m_track->signal_key_moved().
	connect(sigc::hide(sigc::hide(update)));
      track_mode_changed(m_track->get_mode());
    }
    else
      track_mode_changed(Dino::Track::NormalMode);
  }
}


void OctaveLabel::track_mode_changed(Dino::Track::Mode mode) {
  if (mode == Dino::Track::NormalMode)
    set_size_request(m_width, 128 * m_note_height + 1);
  else if (mode == Dino::Track::DrumMode)
    set_size_request(100, m_track->get_keys().size() * m_drum_height + 1);
  queue_draw();
}
