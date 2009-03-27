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

#include "command.hpp"
#include "commandproxy.hpp"
#include "debuggingpage.hpp"


using namespace Dino;
using namespace std;


DebuggingPage::DebuggingPage(CommandProxy& proxy)
  : m_proxy(proxy),
    m_list(1, false) {
  m_list.get_column(0)->set_title("Command");
  add(m_list);
  reset_gui();
}
 

void DebuggingPage::reset_gui() {
  m_list.clear_items();
  deque<Command*>::const_iterator iter;
  for (iter = m_proxy.get_undo_stack().begin(); 
       iter != m_proxy.get_undo_stack().end(); ++iter) {
    m_list.append_text((*iter)->get_name());
  }
}
