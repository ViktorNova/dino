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

#ifndef INFOEDITOR_HPP
#define INFOEDITOR_HPP

#include <gtkmm.h>

#include "debug.hpp"
#include "plugininterface.hpp"


namespace Dino {
  class Song;
}


class InfoEditor : public GUIPage {
public:
  
  InfoEditor(const Dino::Song& song, Dino::CommandProxy& proxy);
  
  void reset_gui();
  
protected:
  
  void update_info(const std::string& info);
  
  Gtk::Entry* m_ent_title;
  Gtk::Entry* m_ent_author;
  Gtk::TextView* m_text_info;
  
  const Dino::Song& m_song;
  Dino::CommandProxy& m_proxy;
};


#endif
