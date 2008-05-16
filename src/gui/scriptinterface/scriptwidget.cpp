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

#include <cstring>

#include <gtkmm/textbuffer.h>

#include "scriptwidget.hpp"


using namespace Glib;
using namespace Gtk;
using namespace std;
using namespace sigc;
using namespace Dino;


namespace {
  ScriptWidget* m_sw;
  PluginInterface* m_plif;
}


extern "C" {
  string dino_get_name() { 
    return "Script interface"; 
  }
  
  void dino_load_plugin(PluginInterface& plif) {
    m_plif = &plif;
    m_sw = manage(new ScriptWidget);
    plif.add_page("Scripting", *m_sw);
  }
  
  void dino_unload_plugin() {
    m_plif->remove_page(*m_sw);
  }
}


ScriptWidget::ScriptWidget() {
  m_vte = vte_terminal_new();
  set_border_width(3);
  pack_start(*manage(wrap(m_vte)));
  vte_terminal_set_visible_bell(VTE_TERMINAL(m_vte), TRUE);
  vte_terminal_feed(VTE_TERMINAL(m_vte), 
		    "\033[32;1mWelcome to Dino!\033[0m\n\r", 29);
  vte_terminal_feed(VTE_TERMINAL(m_vte),
		    "Launching Python interpreter...\n\r", 33);

  string interpreter = INTERPRETERDIR "/Python.interpreter";
  char** argv = static_cast<char**>(calloc(3, sizeof(char*)));
  argv[0] = strdup(interpreter.c_str());
  argv[1] = strdup(m_plif->get_dbus_name().c_str());
  argv[2] = 0;
  char** envv = { 0 };
  vte_terminal_fork_command(VTE_TERMINAL(m_vte), interpreter.c_str(), 
			    argv, envv, INTERPRETERDIR, FALSE, FALSE, FALSE);
}
