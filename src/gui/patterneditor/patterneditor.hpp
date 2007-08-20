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

#ifndef PATTERNEDITOR_HPP
#define PATTERNEDITOR_HPP

#include <gtkmm.h>

#include "curveeditor.hpp"
#include "debug.hpp"
#include "octavelabel.hpp"
#include "noteeditor.hpp"
#include "plugininterface.hpp"
#include "ruler.hpp"
#include "singletextcombo.hpp"


namespace Dino {
  class CommandProxy;
  class Song;
}

class PatternDialog;


class PatternEditor : public GUIPage {
public:
  
  PatternEditor(Dino::CommandProxy& proxy);
  
  void reset_gui();
  
  void cut_selection();
  void copy_selection();
  void paste();
  void delete_selection();
  void select_all();
  
protected:
  
  void pattern_added(int id);
  
  void update_track_combo();
  void update_pattern_combo();
  void update_controller_combo();
  
  void set_active_track(int track);
  void set_active_pattern(int pattern);
  void set_active_controller(long pattern);
  
  void add_pattern();
  void delete_pattern();
  void duplicate_pattern();
  void edit_pattern_properties();

  void add_toolbutton(Gtk::Toolbar* tbar, Gtk::ToolButton*& tbutton, 
                      Gtk::BuiltinStockID stock, const std::string& tip,
                      void (PatternEditor::*button_slot)());

  SingleTextCombo m_cmb_track;
  SingleTextCombo m_cmb_pattern;
  SingleTextCombo m_cmb_controller;
  PatternRuler m_pattern_ruler;
  OctaveLabel m_octave_label;

  NoteEditor m_ne;
  CurveEditor m_cce;

  PatternDialog* m_dlg_pattern;

  Gtk::Tooltips m_tooltips;

  Gtk::ToolButton* m_tbn_add_pattern;
  Gtk::ToolButton* m_tbn_delete_pattern;
  Gtk::ToolButton* m_tbn_duplicate_pattern;
  Gtk::ToolButton* m_tbn_set_pattern_properties;

  sigc::connection m_track_combo_connection;
  sigc::connection m_pattern_combo_connection;
  sigc::connection m_conn_pat_added;
  sigc::connection m_conn_pat_removed;
  sigc::connection m_conn_cont_added;
  sigc::connection m_conn_cont_removed;

  int m_active_track;
  int m_active_pattern;
  long m_active_controller;

  Dino::CommandProxy& m_proxy;
  const Dino::Song& m_song;

};


#endif
