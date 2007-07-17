/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006-2007  Lars Luthman <lars.luthman@gmail.com>
   
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

#include "song.hpp"
#include "songcommands.hpp"


namespace Dino {

  
  SetSongTitle::SetSongTitle(Song& song, const std::string& title)
    : Command("Change song title"),
      m_song(song),
      m_new(title) {

  }

  
  bool SetSongTitle::do_command() {
    m_old = m_song.get_title();
    m_song.set_title(m_new);
  }
    
  
  bool SetSongTitle::undo_command() {
    m_song.set_title(m_old);
  }


}
