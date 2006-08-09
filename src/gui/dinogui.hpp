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
   along with this program; if not, write to the Free Software Foundation, 
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#ifndef DINOGUI_HPP
#define DINOGUI_HPP

#include <gtkmm.h>
#include <libglademm.h>
#include <lash/lash.h>

#include "cceditor.hpp"
#include "debug.hpp"
#include "octavelabel.hpp"
#include "patterneditor.hpp"
#include "plugindialog.hpp"
#include "plugininterfaceimplementation.hpp"
#include "pluginlibrary.hpp"
#include "ringbuffer.hpp"
#include "ruler.hpp"
#include "sequencer.hpp"
#include "singletextcombo.hpp"
#include "song.hpp"


class GUIPage;


/** This is the main class. It connects our custom widgets to the rest of the
    GUI and sets up all signals and initial values. */
class DinoGUI {
public:
  DinoGUI(int argc, char** argv, Glib::RefPtr<Gnome::Glade::Xml> xml);
  
  Gtk::Window* get_window();
  
  int add_page(const std::string& label, GUIPage& page);
  void remove_page(GUIPage& page);
  void remove_page(int pagenum);
  
private:

  /// @name Menu and toolbutton callbacks
  //@{
  void slot_file_open();
  void slot_file_save();
  void slot_file_save_as();
  void slot_file_clear_all();
  void slot_file_quit();
  
  void slot_edit_cut();
  void slot_edit_copy();
  void slot_edit_paste();
  void slot_edit_delete();
  void slot_edit_select_all();
  
  void slot_transport_play();
  void slot_transport_stop();
  void slot_transport_go_to_start();
  
  void slot_plugins_manage();
  
  void slot_help_about_dino();
  //@}
  
  void load_plugins(int argc, char** argv);
  
  /** This is a convenience function that returns a pointer of type @c T* to
      the widget with name @c name. If there is no widget in @c xml with that
      name it returns 0. */
  template <class T>
  inline T* w(Glib::RefPtr<Gnome::Glade::Xml>& xml, const string& name) {
    using namespace Dino;
    T* widget = dynamic_cast<T*>(xml->get_widget(name));
    if (widget == 0)
      dbg0<<"Could not load widget "<<name<<" of type "
	  <<demangle(typeid(T).name())<<endl;
    return widget;
  }
  
  /** This is a convenience function that returns a pointer of type @c T* to
      the widget with name @c name. If there is no widget in @c xml with that
      name it returns 0. */
  template <class T>
  inline T* wd(Glib::RefPtr<Gnome::Glade::Xml>& xml, const string& name) {
    using namespace Dino;
    T* widget = xml->get_widget_derived(name, widget);
    if (widget == 0)
      dbg0<<"Could not load derived widget "<<name<<" of type "
	  <<demangle(typeid(T).name())<<endl;
    return widget;
  }
  
  void reset_gui();
  void init_menus(Glib::RefPtr<Gnome::Glade::Xml>& xml);
  bool init_lash(int argc, char** argv);
  
  // internal callbacks
  bool slot_check_ladcca_events();
  void page_switched(guint index);
  
  Dino::Song m_song;
  
  Gtk::Window* m_window;
  std::map<std::string, Gtk::MenuItem*> m_menuitems;
  
  Gtk::Notebook* m_nb;
  
  Gtk::AboutDialog m_about_dialog;
  PluginDialog m_plug_dialog;
  
  Dino::Sequencer m_seq;
  
  lash_client_t* m_lash_client;
  
  PluginInterfaceImplementation m_plif;
  PluginLibrary m_plib;
};


#endif


// For the Doxygen main page:
/** @mainpage Dino source code documentation
    This source code documentation is generated in order to help people
    (including me) understand how Dino works, or is supposed to work. 
    But it's not meant to be a complete API reference, so many functions and
    types will be undocumented.
    
    Send questions and comments to larsl@users.sourceforge.net. 
*/
