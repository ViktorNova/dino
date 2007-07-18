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

#ifndef SEQUENCEWIDGET_HPP
#define SEQUENCEWIDGET_HPP

#include <gtkmm.h>


namespace Dino {
  class CommandProxy;
  class Song;
  class Track;
}

class PluginInterface;


class SequenceWidget : public Gtk::DrawingArea {
public:
  
  SequenceWidget(Dino::CommandProxy& proxy);
  
  void set_track(Dino::Track* track);

  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  
  void update();
  
  void set_current_beat(int beat);
  void update_menu(PluginInterface& plif);

  sigc::signal<void, int> signal_clicked;
  
private:
  
  void slot_insert_pattern(int pattern, int position);
  void slot_length_changed(int length);
  
  Dino::CommandProxy& m_proxy;
  Dino::Track* m_track;
  int m_col_width;

  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Colormap> m_colormap;
  Gdk::Color m_bg_color, m_bg_color2, m_fg_color, m_grid_color, m_edge_color, 
    m_hl_color;
  
  int m_drag_seqid;
  //int m_drag_beat, m_drag_pattern;
  
  int m_current_beat;
  
  Gtk::Menu m_pattern_menu;
  Gtk::Menu m_action_menu;
};


#endif
