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

#include <gtkmm/box.h>

#include "command.hpp"
#include "commandproxy.hpp"
#include "debuggingpage.hpp"


using namespace Dino;
using namespace std;


DebuggingPage::DebuggingPage(CommandProxy& proxy)
  : m_proxy(proxy),
    m_stacklist(1, false),
    m_dbglist(4, false) {
  
  m_stacklist.get_column(0)->set_title("Undoable commands");
  
  m_dbglist.get_column(0)->set_title("Level");
  m_dbglist.get_column(1)->set_title("Source file");
  m_dbglist.get_column(2)->set_title("Code line");
  m_dbglist.get_column(3)->set_title("Message");
  
  Gtk::VBox* vbox = manage(new Gtk::VBox);
  add(*vbox);
  
  Gtk::ScrolledWindow* stackscrw = manage(new Gtk::ScrolledWindow);
  stackscrw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  vbox->pack_start(*stackscrw);
  stackscrw->add(m_stacklist);

  Gtk::ScrolledWindow* dbgscrw = manage(new Gtk::ScrolledWindow);
  dbgscrw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  vbox->pack_start(*dbgscrw);
  dbgscrw->add(m_dbglist);
  
  signal_debug.connect(sigc::mem_fun(*this, &DebuggingPage::add_debug_msg));
  
  reset_gui();
}
 

void DebuggingPage::reset_gui() {
  m_stacklist.clear_items();
  deque<Command*>::const_iterator iter;
  for (iter = m_proxy.get_undo_stack().begin(); 
       iter != m_proxy.get_undo_stack().end(); ++iter) {
    m_stacklist.append_text((*iter)->get_name());
  }
}


void DebuggingPage::add_debug_msg(unsigned level, const std::string& file,
				  unsigned line, const std::string& msg) {
  guint row = m_dbglist.append_text(cc+ level);
  m_dbglist.set_text(row, 1, file);
  m_dbglist.set_text(row, 2, cc+ line);
  m_dbglist.set_text(row, 3, msg);
}
