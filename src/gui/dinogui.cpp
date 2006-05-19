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

#include "cceditor.hpp"
#include "controller_numbers.hpp"
#include "controllerdialog.hpp"
#include "deleter.hpp"
#include "dinogui.hpp"
#include "evilscrolledwindow.hpp"
#include "pattern.hpp"
#include "patterndialog.hpp"
#include "patterneditor.hpp"
#include "plugindialog.hpp"
#include "plugininterfaceimplementation.hpp"
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
  : m_seq("Dino", m_song),
    m_plif(*this, m_song, m_seq),
    m_plib(m_plif) {
  
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
  
  // initialise the "About" dialog
  m_about_dialog = w<AboutDialog>(xml, "dlg_about");
  m_about_dialog->set_copyright("\u00A9 " CR_YEAR " Lars Luthman "
				"<larsl@users.sourceforge.net>");
  m_about_dialog->set_version(VERSION);
  
  // initialise the "Plugins" dialog
  m_plug_dialog = xml->get_widget_derived("dlg_plugins", m_plug_dialog);
  m_plug_dialog->set_library(m_plib);
  w<Button>(xml, "dlg_plugins_close")->signal_clicked().
    connect(mem_fun(*m_plug_dialog, &Dialog::hide));
  
  m_nb = w<Notebook>(xml, "main_notebook");
  m_nb->signal_switch_page().
    connect(sigc::hide<0>(mem_fun(*this, &DinoGUI::page_switched)));
  
  init_menus(xml);
  
  load_plugins(argc, argv);
  
  reset_gui();
  
  m_window->show_all();
}


Gtk::Window* DinoGUI::get_window() {
  return m_window;
}


int DinoGUI::add_page(const std::string& label, GUIPage& page) {
  int result = m_nb->append_page(page, label);
  m_window->show_all();
  return result;
}


void DinoGUI::remove_page(GUIPage& page) {
  m_nb->remove_page(page);
}


void DinoGUI::remove_page(int pagenum) {
  m_nb->remove_page(pagenum);
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
  GUIPage* page = 
    dynamic_cast<GUIPage*>(m_nb->get_nth_page(m_nb->get_current_page()));
  assert(page);
  page->cut_selection();
}


void DinoGUI::slot_edit_copy() {
  GUIPage* page = 
    dynamic_cast<GUIPage*>(m_nb->get_nth_page(m_nb->get_current_page()));
  assert(page);
  page->copy_selection();
}


void DinoGUI::slot_edit_paste() {
  GUIPage* page = 
    dynamic_cast<GUIPage*>(m_nb->get_nth_page(m_nb->get_current_page()));
  assert(page);
  page->paste();
}


void DinoGUI::slot_edit_delete() {
  GUIPage* page = 
    dynamic_cast<GUIPage*>(m_nb->get_nth_page(m_nb->get_current_page()));
  assert(page);
  page->delete_selection();
}


void DinoGUI::slot_edit_select_all() {
  GUIPage* page = 
    dynamic_cast<GUIPage*>(m_nb->get_nth_page(m_nb->get_current_page()));
  assert(page);
  page->select_all();
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
  std::list<Widget*> pages = m_nb->get_children();
  std::list<Widget*>::iterator iter;
  for (iter = pages.begin(); iter != pages.end(); ++iter) {
    GUIPage* page = dynamic_cast<GUIPage*>(*iter);
    assert(page);
    page->reset_gui();
  }
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
  menuSlots["plugins_manage_plugins"] = &DinoGUI::slot_plugins_manage;

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
  m_lash_client = lash_init(lash_extract_args(&argc, &argv), "dino", 
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
  GUIPage* page = dynamic_cast<GUIPage*>(m_nb->get_nth_page(index));
  if (!page)
    return;
  bool clipboard_active = page->get_flags() & GUIPage::PageSupportsClipboard;
  m_menuitems["edit_cut"]->set_sensitive(clipboard_active);
  m_menuitems["edit_copy"]->set_sensitive(clipboard_active);
  m_menuitems["edit_paste"]->set_sensitive(clipboard_active);
  m_menuitems["edit_delete"]->set_sensitive(clipboard_active);
  m_menuitems["edit_select_all"]->set_sensitive(clipboard_active);
}


void DinoGUI::slot_plugins_manage() {
  m_plug_dialog->present();
}


void DinoGUI::load_plugins(int argc, char** argv) {
  PluginLibrary::iterator iter;
  iter = m_plib.find("Core actions");
  if (iter != m_plib.end())
    m_plib.load_plugin(iter);
  iter = m_plib.find("Sequence editor");
  if (iter != m_plib.end())
    m_plib.load_plugin(iter);
  iter = m_plib.find("Pattern editor");
  if (iter != m_plib.end())
    m_plib.load_plugin(iter);
  iter = m_plib.find("Info editor");
  if (iter != m_plib.end())
    m_plib.load_plugin(iter);
}
