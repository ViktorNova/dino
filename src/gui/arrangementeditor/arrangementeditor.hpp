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

#ifndef ARRANGEMENTEDITOR_HPP
#define ARRANGEMENTEDITOR_HPP

#include <map>

#include <gtkmm.h>

#include "plugininterface.hpp"
#include "debug.hpp"
#include "ruler.hpp"
#include "song.hpp"
#include "trackdialog.hpp"


namespace Dino {
  class Sequencer;
}

class PluginInterface;
class TrackLabel;
class TrackWidget;


class ArrangementEditor : public GUIPage {
public:
  
  ArrangementEditor(PluginInterface& plif);
  
protected:
  

  struct SingleTrackGUI {
    SingleTrackGUI(TrackLabel* l = 0, TrackWidget* w = 0) 
      : label(l), widget(w) { }
    TrackLabel* label;
    TrackWidget* widget;
  };
  

  void add_track();
  void delete_track();
  void edit_track_properties();
  void play();
  void stop();
  void go_to_start();
  void ruler_clicked(const Dino::SongTime& time, int button);
  void track_added(int track);
  void track_removed(int track);
  
  void set_active_track(int track);
  
  void add_toolbutton(Gtk::Toolbar* tbar, Gtk::ToolButton*& tbutton, 
                      Gtk::BuiltinStockID stock, const std::string& tip,
                      void (ArrangementEditor::*button_slot)());
  
  void set_recording_track(Dino::Song::ConstTrackIterator iter);
  
  sigc::signal<void, int> signal_active_track_changed_internal;

  // external references
  PluginInterface& m_plif;
  Dino::Sequencer& m_seq;
  Dino::CommandProxy& m_proxy;
  const Dino::Song& m_song;
  
  // GUI components
  Ruler m_sequence_ruler;
  Gtk::VBox m_vbx_track_editor;
  Gtk::VBox m_vbx_track_labels;
  Gtk::ToolButton* m_tbn_add_track;
  Gtk::ToolButton* m_tbn_delete_track;
  Gtk::ToolButton* m_tbn_edit_track_properties;
  Gtk::ToolButton* m_tbn_play;
  Gtk::ToolButton* m_tbn_stop;
  Gtk::ToolButton* m_tbn_go_to_start;
  Gtk::SpinButton m_spb_song_length;
  Gtk::Tooltips m_tooltips;
  TrackDialog m_dlg_track;
  
  // state
  std::map<int, SingleTrackGUI> m_track_map;
  int m_active_track;

};


#endif
