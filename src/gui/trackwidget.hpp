/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
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

#ifndef TRACKWIDGET_HPP
#define TRACKWIDGET_HPP

#include <gtkmm.h>

#include "curveeditor.hpp"
#include "sequencewidget.hpp"


namespace Dino {
  class Track;
}


class PluginInterface;


class TrackWidget : public Gtk::VBox {
public:
  
  TrackWidget();
  ~TrackWidget();
  
  void set_track(Dino::Track* track);

  void update();
  
  void set_current_beat(int beat);
  void update_menu(PluginInterface& plif);

  sigc::signal<void, int> signal_clicked;
  sigc::signal<void, const std::string&> signal_status;
  
private:
  
  void controller_added(long number, Dino::Track* track);
  void controller_removed(long number, Dino::Track* track);
  
  
  SequenceWidget m_swdg;
  CurveEditor m_cce;

  sigc::connection m_ctrl_added_connection;
  sigc::connection m_ctrl_removed_connection;
  
};


#endif
