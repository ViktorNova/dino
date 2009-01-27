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

#ifndef RULER_HPP
#define RULER_HPP

#include <gtkmm.h>

#include "songtime.hpp"


namespace Dino {
  class Song;
}


class Ruler : public Gtk::DrawingArea {
public:
  Ruler(const Dino::SongTime& length, int subs, int interval, 
	Dino::SongTime::Tick ticks_per_pixel, int height);
  
  void set_length(const Dino::SongTime& length);
  void set_subdivisions(int subdivisions);
  void set_interval(int interval);
  void set_ticks_per_pixel(Dino::SongTime::Tick ticks_per_pixel);
  void set_loop_start(const Dino::SongTime& start);
  void set_loop_end(const Dino::SongTime& end);
  
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* event);

public:
  
  sigc::signal<void, const Dino::SongTime&, int> signal_clicked;
  
private:
  
  Dino::SongTime pixel2time(int x);
  int time2pixel(const Dino::SongTime& time);

  
  Dino::SongTime m_length;
  int m_subs;
  int m_interval;
  Dino::SongTime::Tick m_ticks_per_pixel;
  int m_height;
  Dino::SongTime m_loop_start;
  Dino::SongTime m_loop_end;
  
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
