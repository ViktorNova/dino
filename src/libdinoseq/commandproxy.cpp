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

#include <iostream>

#include "commandproxy.hpp"
#include "song.hpp"
#include "genericcommands.hpp"
#include "sequencer.hpp"


namespace Dino {


  CommandProxy::CommandProxy(Song& song, Sequencer& seq)
    : m_song(song),
      m_seq(seq),
      m_active(false) {

  }
    
  
  bool CommandProxy::can_undo() const {
    return !m_stack.empty();
  }
  
  
  std::string CommandProxy::get_next_undo_name() const {
    if (m_stack.empty())
      return "";
    return m_stack.top()->get_name();
  }
  

  bool CommandProxy::undo() {
    if (m_active)
      return false;
    m_active = true;
    std::cerr<<__PRETTY_FUNCTION__<<std::endl;
    if (m_stack.empty())
      return false;
    Command* cmd = m_stack.top();
    m_stack.pop();
    cmd->undo_command();
    delete cmd;
    if (m_stack.empty())
      std::cerr<<"Stack is empty!"<<std::endl;
    else
      std::cerr<<"Stack is not empty!"<<std::endl;
    m_signal_stack_changed();
    m_active = false;
    return true;
  }
    
  
  void CommandProxy::play() {
    m_seq.play();
  }
  
  
  void CommandProxy::stop() {
    m_seq.stop();
  }
  
  
  void CommandProxy::go_to_beat(double beat) {
    m_seq.go_to_beat(beat);
  }
  
  
  bool CommandProxy::set_song_title(const std::string& title) {
    return push_and_do(new SetString<Song>("Change song title", 
					   m_song, title,
					   &Song::get_title, 
					   &Song::set_title));
  }
  
  
  bool CommandProxy::set_song_author(const std::string& author) {
    return push_and_do(new SetString<Song>("Change song author", 
					   m_song, author,
					   &Song::get_author, 
					   &Song::set_author));
  }
  
  
  bool CommandProxy::set_song_info(const std::string& info) {
    return push_and_do(new SetString<Song>("Change song info", 
					   m_song, info,
					   &Song::get_info, 
					   &Song::set_info));
  }
  
  
  sigc::signal<void>& CommandProxy::signal_stack_changed() {
    return m_signal_stack_changed;
  }
  

  bool CommandProxy::push_and_do(Command* cmd) {
    if (m_active) {
      delete cmd;
      return false;
    }
    m_active = true;
    m_stack.push(cmd);
    cmd->do_command();
    m_signal_stack_changed();
    m_active = false;
    return true;
  }
  
  
}
