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

#include "song.hpp"
#include "tempolabel.hpp"


using namespace std;
using namespace Dino;
using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace Pango;


TempoLabel::TempoLabel(const Song* song) 
  : m_song(song), m_width(122), m_height(20) {
  assert(song);
  m_colormap  = Colormap::get_system();
  m_fg_color.set_rgb(0, 0, 0);
  m_colormap->alloc_color(m_fg_color);
  m_layout = Layout::create(get_pango_context());
  m_layout->set_text("BPM");
  set_size_request(m_width, m_height);
}
  

void TempoLabel::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color);
  FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  win->clear();
}


bool TempoLabel::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  //win->clear();
  m_gc->set_foreground(m_fg_color);
  int lHeight = m_layout->get_pixel_logical_extents().get_height();
  win->draw_layout(m_gc, 24, (m_height - lHeight)/2, m_layout);
  return true;
}


void TempoLabel::update() {
  RefPtr<Gdk::Window> win = get_window();
  if (win) {
    win->invalidate_rect(Gdk::Rectangle(0, 0, m_width, m_height), false);
    win->process_updates(false);
  }
}


