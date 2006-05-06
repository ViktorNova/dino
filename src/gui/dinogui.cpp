/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <larsl@users.sourceforge.net>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <sstream>
#include <utility>

#include <gtkmm/messagedialog.h>

#include <config.hpp>

#include "cceditor.hpp"
#include "controller_numbers.hpp"
#include "controllerdialog.hpp"
#include "deleter.hpp"
#include "dinogui.hpp"
#include "evilscrolledwindow.hpp"
#include "infoeditor.hpp"
#include "pattern.hpp"
#include "patterndialog.hpp"
#include "patterneditor.hpp"
#include "ruler.hpp"
#include "sequenceeditor.hpp"
#include "song.hpp"
#include "tempolabel.hpp"
#include "tempowidget.hpp"
#include "track.hpp"


using namespace sigc;
using namespace Gtk;
using namespace Gdk;
using namespace Glib;
using namespace Gnome::Glade;
using namespace Dino;


DinoGUI::DinoGUI(int argc, char** argv, RefPtr<Xml> xml) 
  : m_seq("Dino", m_song) {
  
  if (!m_seq.is_valid()) {
    MessageDialog dlg("Could not initialise the sequencer! You will not be "
		      "able to play anything.", false, MESSAGE_WARNING);
    dlg.run();
  }
  
  if (!init_lash(argc, argv)) {
    MessageDialog dlg("Could not initialise LASH! You will not be able "
		      "to save your session.", false, MESSAGE_WARNING);
    dlg.run();
  }
  
  m_window = w<Gtk::Window>(xml, "main_window");
  m_about_dialog = w<AboutDialog>(xml, "dlg_about");
  m_about_dialog->set_copyright("\u00A9 " CR_YEAR " Lars Luthman "
				"<larsl@users.sourceforge.net>");
  m_about_dialog->set_version(PACKAGE_VERSION);
  
  Notebook* nb = w<Notebook>(xml, "main_notebook");
  m_pe = wd<PatternEditor>(xml, "patternVBox");
  m_pe->set_song(&m_song);
  m_se = manage(new SequenceEditor);
  m_se->set_song(&m_song);
  m_se->set_sequencer(&m_seq);
  nb->append_page(*m_se, "Arrangement");
  m_ie = manage(new InfoEditor);
  m_ie->set_song(&m_song);
  nb->append_page(*m_ie, "Information");
  
  nb->signal_switch_page().
    connect(sigc::hide<0>(mem_fun(*this, &DinoGUI::page_switched)));
  
  init_menus(xml);
  reset_gui();
  
  m_window->show_all();
}


Gtk::Window* DinoGUI::get_window() {
  return m_window;
}


void DinoGUI::slot_file_open() {
  m_seq.stop();
  m_song.load_file("output.dino");
  reset_gui();
  m_seq.reset_ports();
  m_seq.go_to_beat(0);
}


void DinoGUI::slot_file_save() {
  m_song.write_file("output.dino");
}


void DinoGUI::slot_file_save_as() {
  m_song.write_file("output.dino");
}


void DinoGUI::slot_file_clear_all() {
  m_song.clear();
  m_song.set_length(32);
  reset_gui();
}


void DinoGUI::slot_file_quit() {
  m_seq.stop();
  Main::quit();
}


void DinoGUI::slot_edit_cut() {
  m_pe->cut_selection();
}


void DinoGUI::slot_edit_copy() {
  m_pe->copy_selection();
}


void DinoGUI::slot_edit_paste() {
  m_pe->paste();
}


void DinoGUI::slot_edit_delete() {
  m_pe->delete_selection();
}


void DinoGUI::slot_edit_select_all() {
  m_pe->select_all();
}


void DinoGUI::slot_transport_play() {
  m_seq.play();
}


void DinoGUI::slot_transport_stop() {
  m_seq.stop();
}


void DinoGUI::slot_transport_go_to_start() {
  m_seq.go_to_beat(0);
}


void DinoGUI::slot_help_about_dino() {
  assert(m_about_dialog != 0);
  m_about_dialog->run();
  m_about_dialog->hide();
}


void DinoGUI::reset_gui() {
  m_pe->reset_gui();
  m_se->reset_gui();
  m_ie->reset_gui();
}


void DinoGUI::init_menus(RefPtr<Xml>& xml) {
  map<string, void (DinoGUI::*)(void)> menuSlots;
  menuSlots["file_clear_all"] = &DinoGUI::slot_file_clear_all;
  menuSlots["file_quit"] = &DinoGUI::slot_file_quit;
  menuSlots["edit_cut"] = &DinoGUI::slot_edit_cut;
  menuSlots["edit_copy"] = &DinoGUI::slot_edit_copy;
  menuSlots["edit_paste"] = &DinoGUI::slot_edit_paste;
  menuSlots["edit_delete"] = &DinoGUI::slot_edit_delete;
  menuSlots["edit_select_all"] = &DinoGUI::slot_edit_select_all;
  menuSlots["transport_play"] = &DinoGUI::slot_transport_play;
  menuSlots["transport_stop"] = &DinoGUI::slot_transport_stop;
  menuSlots["transport_go_to_start"] = &DinoGUI::slot_transport_go_to_start;
  menuSlots["help_about"] = &DinoGUI::slot_help_about_dino;
  map<string, void (DinoGUI::*)(void)>::const_iterator iter;
  for (iter = menuSlots.begin(); iter != menuSlots.end(); ++iter) {
    MenuItem* mi = w<MenuItem>(xml, iter->first);
    assert(mi);
    mi->signal_activate().connect(mem_fun(*this, iter->second));
    m_menuitems[iter->first] = mi;
  }
}


bool DinoGUI::init_lash(int argc, char** argv) {
  dbg1<<"Initialising LASH client"<<endl;
  m_lash_client = lash_init(lash_extract_args(&argc, &argv), PACKAGE_NAME, 
			     LASH_Config_File, LASH_PROTOCOL(2, 0));
  
  if (m_lash_client) {
    lash_event_t* event = lash_event_new_with_type(LASH_Client_Name);
    lash_event_set_string(event, "Dino");
    lash_send_event(m_lash_client, event);			
    lash_jack_client_name(m_lash_client, "Dino");
    signal_timeout().
      connect(mem_fun(*this, &DinoGUI::slot_check_ladcca_events), 500);
  }
  else
    dbg0<<"Could not initialise LASH!"<<endl;
  return (m_lash_client != 0);
}


bool DinoGUI::slot_check_ladcca_events() {
  lash_event_t* event;
  while ((event = lash_get_event(m_lash_client))) {
    
    // save
    if (lash_event_get_type(event) == LASH_Save_File) {
      if (m_song.write_file(string(lash_event_get_string(event)) + "/song")) {
	lash_send_event(m_lash_client, 
			lash_event_new_with_type(LASH_Save_File));
      }
    }
    
    // restore
    else if (lash_event_get_type(event) == LASH_Restore_File) {
      if (m_song.load_file(string(lash_event_get_string(event)) + "/song")) {
	reset_gui();
	lash_send_event(m_lash_client,
			lash_event_new_with_type(LASH_Restore_File));
      }
    }
    
    // quit
    else if (lash_event_get_type(event) == LASH_Quit) {
      Main::instance()->quit();
    }
    
    lash_event_destroy(event);
  }
  return true;
}


void DinoGUI::page_switched(guint index) {
  // ugly hack - what if we reorder the notebook pages?
  bool clipboard_active = (index == 1);
  m_menuitems["edit_cut"]->set_sensitive(clipboard_active);
  m_menuitems["edit_copy"]->set_sensitive(clipboard_active);
  m_menuitems["edit_paste"]->set_sensitive(clipboard_active);
  m_menuitems["edit_delete"]->set_sensitive(clipboard_active);
  m_menuitems["edit_select_all"]->set_sensitive(clipboard_active);
}
