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

#ifndef DINOGUI_HPP
#define DINOGUI_HPP

#include <gtkmm.h>
#include <libglademm.h>
#include <lash/lash.h>

#include "cceditor.hpp"
#include "debug.hpp"
#include "octavelabel.hpp"
#include "patterneditor.hpp"
#include "ringbuffer.hpp"
#include "ruler.hpp"
#include "sequencer.hpp"
#include "singletextcombo.hpp"
#include "song.hpp"


class ControllerDialog;
class PatternDialog;
class TrackDialog;


/** This is the main class. It connects our custom widgets to the rest of the
    GUI and sets up all signals and initial values. */
class DinoGUI {
public:
  DinoGUI(int argc, char** argv, Glib::RefPtr<Gnome::Glade::Xml> xml);
  
  Gtk::Window* get_window();
  
private:

  /// @name Menu and toolbutton callbacks
  //@{
  void slot_file_new();
  void slot_file_open();
  void slot_file_save();
  void slot_file_save_as();
  void slot_file_clear_all();
  void slot_file_quit();
  
  void slot_edit_cut();
  void slot_edit_copy();
  void slot_edit_paste();
  void slot_edit_delete();
  void slot_edit_add_track();
  void slot_edit_delete_track();
  void slot_edit_edit_track_properties();
  void slot_edit_add_pattern();
  void slot_edit_delete_pattern();
  void slot_edit_duplicate_pattern();
  void slot_edit_edit_pattern_properties();
  void slot_edit_add_controller();
  void slot_edit_delete_controller();
  void slot_edit_set_song_length(int song_length);
  
  void slot_transport_play();
  void slot_transport_stop();
  void slot_transport_go_to_start();
  
  void slot_help_about_dino();
  //@}
  
  /** This is a convenience function that returns a pointer of type @c T* to
      the widget with name @c name. If there is no widget in @c xml with that
      name it returns NULL. */
  template <class T>
  inline T* w(const string& name) {
    using namespace Dino;
    T* widget = dynamic_cast<T*>(m_xml->get_widget(name));
    if (widget == NULL)
      dbg0<<"Could not load widget "<<name<<" of type "
	  <<demangle(typeid(T).name())<<endl;
    return widget;
  }
  
  void reset_gui();
  void update_track_widgets();
  void update_track_combo();
  void update_pattern_combo();
  void update_controller_combo();
  void init_pattern_editor();
  void init_sequence_editor();
  void init_info_editor();
  void init_menus();
  bool init_lash(int argc, char** argv);
  
  // internal callbacks
  bool slot_check_ladcca_events();
  void set_active_track(int active_track);
  void set_active_pattern(int active_pattern);
  void set_active_controller(int active_controller);
  
  // internal signals
  sigc::signal<void, int> signal_active_track_changed;
  sigc::signal<void, int, int> signal_active_pattern_changed;
  sigc::signal<void, int> signal_active_controller_changed;
  
  Dino::Song m_song;
  int m_active_track;
  int m_active_pattern;
  long m_active_controller;
  
  Glib::RefPtr<Gnome::Glade::Xml> m_xml;
  Gtk::Window* m_window;
  PatternEditor m_pe;
  CCEditor m_cce;
  Gtk::VBox* m_vbx_track_editor;
  Gtk::VBox* m_vbx_track_labels;
  
  SingleTextCombo m_cmb_track;
  SingleTextCombo m_cmb_pattern;
  SingleTextCombo m_cmb_controller;
  
  std::map<std::string, Gtk::ToolButton*> m_toolbuttons;
  std::map<std::string, Gtk::MenuItem*> m_menuitems;

  sigc::connection m_track_combo_connection;
  sigc::connection m_pattern_combo_connection;
  sigc::connection m_conn_pat_added;
  sigc::connection m_conn_pat_removed;
  sigc::connection m_conn_cont_added;
  sigc::connection m_conn_cont_removed;
  Gtk::SpinButton* m_sb_cc_editor_size;
  Ruler m_sequence_ruler;
  PatternRuler m_pattern_ruler_1;
  OctaveLabel m_octave_label;
  Gtk::Entry* m_ent_title;
  Gtk::Entry* m_ent_author;
  Gtk::TextView* m_text_info;
  
  TrackDialog* m_dlg_track;
  PatternDialog* m_dlg_pattern;
  ControllerDialog* m_dlg_controller;
  
  Gtk::AboutDialog* m_about_dialog;
  
  Dino::Sequencer m_seq;
  
  lash_client_t* m_lash_client;
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
