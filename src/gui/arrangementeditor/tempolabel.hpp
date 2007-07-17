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

#ifndef TEMPOLABEL_HPP
#define TEMPOLABEL_HPP

#include <gtkmm.h>


namespace Dino {
  class Song;
}


class TempoLabel : public Gtk::DrawingArea {
public:
  
  TempoLabel(const Dino::Song* song = 0);
  
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  
  void update();
  
private:
  
  const Dino::Song* m_song;
  int m_width, m_height;

  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Colormap> m_colormap;
  Gdk::Color m_bg_color, m_fg_color;
  Glib::RefPtr<Pango::Layout> m_layout;
};


#endif
