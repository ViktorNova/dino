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

#include "commandproxy.hpp"
#include "curveeditor.hpp"
#include "track.hpp"
#include "trackwidget.hpp"
#include "debug.hpp"


using namespace Dino;
using namespace std;


TrackWidget::TrackWidget(CommandProxy& proxy)
  : m_swdg(proxy) {
  
  m_cce.set_step_width(20);
  m_cce.set_alternation(4);
  pack_start(m_swdg);
  show_all();
  
  dbg1<<"Created TrackWidget 0x"<<hex<<(int)this<<dec<<endl;
  
  m_cce.signal_status().connect(signal_status);
}


TrackWidget::~TrackWidget() {
  dbg1<<"Destroyed TrackWidget 0x"<<hex<<(int)this<<dec<<endl;
}


void TrackWidget::set_track(Dino::Track* track) {
  
  dbg1<<"set_track(0x"<<hex<<(int)track<<dec<<") for TrackWidget 0x"
      <<hex<<(int)this<<dec<<endl;
  
  m_ctrl_added_connection.disconnect();
  m_ctrl_removed_connection.disconnect();
  m_swdg.set_track(track);
  
  if (!track)
    remove(m_cce);
  
  else {
    m_ctrl_added_connection = track->signal_curve_added().
      connect(bind(mem_fun(*this, &TrackWidget::curve_added), track));
    m_ctrl_removed_connection = track->signal_curve_removed().
      connect(bind(mem_fun(*this, &TrackWidget::curve_removed), track));
    
    bool has_curves = (track->curves_begin() != track->curves_end());
    bool cce_visible = (children().find(m_cce) != children().end());
    
    if (has_curves && !cce_visible) {
      pack_start(m_cce);
      show_all();
    }
    
    else if (!has_curves && cce_visible)
      remove(m_cce);
  }
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


void TrackWidget::curve_added(long number, Dino::Track* track) {
  bool has_curves = (track->curves_begin() != track->curves_end());
  bool cce_visible = (children().find(m_cce) != children().end());
  
  if (has_curves && !cce_visible) {
    m_cce.set_curve(&*track->curves_begin());
    pack_start(m_cce);
    show_all();
  }
}


void TrackWidget::curve_removed(long number, Dino::Track* track) {
  bool has_curves = (track->curves_begin() != track->curves_end());
  bool cce_visible = (children().find(m_cce) != children().end());
  
  if (!has_curves)
    m_cce.set_curve(0);
  else
    m_cce.set_curve(&*track->curves_begin());
  
  if (!has_curves && cce_visible)
    remove(m_cce);
}

