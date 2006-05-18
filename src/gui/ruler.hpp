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

#ifndef RULER_HPP
#define RULER_HPP

#include <gtkmm.h>


namespace Dino {
  class Song;
}


class Ruler : public Gtk::DrawingArea {
public:
  Ruler(int length, int subs, int interval, int size, int height);
  
  void set_length(int length);
  void set_subdivisions(int subs);
  void set_interval(int interval);
  void set_division_size(int size);
  void set_loop_start(int start);
  void set_loop_end(int end);
  
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* event);

public:
  
  sigc::signal<void, double, int> signal_clicked;
  
private:
  
  int m_length;
  int m_subs;
  int m_interval;
  int m_div_size;
  int m_height;
  int m_loop_start;
  int m_loop_end;
  
  Gdk::Color m_fg, m_loop_bg, m_loop_marker;
  
  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Colormap> m_colormap;
  
};


class PatternRuler : public ::Ruler {
public:

  PatternRuler();

  void set_song(const Dino::Song* song);
  void set_pattern(int track, int pattern);

private:

  const Dino::Song* m_song;

};


#endif
