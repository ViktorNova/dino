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

#ifndef TEMPOWIDGET_HPP
#define TEMPOWIDGET_HPP

#include <gtkmm.h>


namespace Dino {
  class CommandProxy;
  class Song;
}


class PluginInterface;


class TempoWidget : public Gtk::DrawingArea {
public:
  
  // XXX what was the purpose of allowing songless tempo widgets again?
  TempoWidget(Dino::CommandProxy& proxy, const Dino::Song* song = 0);
  
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  
  void update();
  void update_menu(PluginInterface& plif);
  
private:
  
  void length_changed(int length);
  
  Dino::CommandProxy& m_proxy;
  const Dino::Song* m_song;
  int m_col_width;

  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Colormap> m_colormap;
  Gdk::Color m_bg_color, m_bg_color2, m_fg_color, 
    m_grid_color, m_edge_color, m_hl_color;
  
  int m_drag_beat;
  int m_drag_start_y;
  int m_editing_bpm;
  const Dino::TempoMap::TempoChange* m_active_tempo;
  
  Gtk::Menu m_menu;
};


#endif
