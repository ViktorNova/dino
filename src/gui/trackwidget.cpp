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

#include "trackwidget.hpp"


TrackWidget::TrackWidget() {
  pack_start(m_swdg);
  show_all();
}
 

void TrackWidget::set_track(Dino::Track* track) {
  m_swdg.set_track(track);
}


void TrackWidget::update() {
  m_swdg.update();
}
  

void TrackWidget::set_current_beat(int beat) {
  m_swdg.set_current_beat(beat);
}


void TrackWidget::update_menu(PluginInterface& plif) {
  m_swdg.update_menu(plif);
}

