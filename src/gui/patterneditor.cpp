/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
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

#include <iostream>

#include "curve.hpp"
#include "controllerdialog.hpp"
#include "controller_numbers.hpp"
#include "evilscrolledwindow.hpp"
#include "patterndialog.hpp"
#include "patterneditor.hpp"
#include "song.hpp"
#include "track.hpp"


using namespace Dino;
using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace std;


namespace {
  PatternEditor* m_pe;
  PluginInterface* m_plif;
}


extern "C" {
  string dino_get_name() { 
    return "Pattern editor"; 
  }
  
  void dino_load_plugin(PluginInterface& plif) {
    m_plif = &plif;
    m_pe = manage(new PatternEditor(plif.get_song()));
    plif.add_page("Patterns", *m_pe);
  }
  
  
  void dino_unload_plugin() { 
    m_plif->remove_page(*m_pe); 
  }
}  


PatternEditor::PatternEditor(Song& song)
  : GUIPage(PageSupportsClipboard),
    m_octave_label(20, 8),
    m_active_track(-1),
    m_active_pattern(-1),
    m_active_controller(-1),
    m_song(song) {
  
  VBox* v = manage(new VBox);
  
  m_tooltips.enable();
  
  // get all the widgets from the glade file
  Scrollbar* hscroll = manage(new HScrollbar);
  Scrollbar* vscroll = manage(new VScrollbar);
  
  // add toolbar
  Toolbar* tbar = manage(new Toolbar);
  tbar->set_toolbar_style(TOOLBAR_ICONS);
  v->pack_start(*tbar, false, true);
  
  Table* table = manage(new Table(6, 3));
  table->set_row_spacings(3);
  table->set_col_spacings(3);
  table->set_border_width(3);
  v->pack_start(*table);
  
  // add track and pattern combos
  HBox* cb_box = manage(new HBox);
  cb_box->set_spacing(5);
  cb_box->pack_start(*manage(new Label("Track:")), false, false);
  cb_box->pack_start(m_cmb_track, true, true);
  cb_box->pack_start(*manage(new Label("Pattern:")), false, false);
  cb_box->pack_start(m_cmb_pattern, true, true);
  table->attach(*cb_box, 1, 5, 0, 1, FILL|EXPAND, FILL);
  
  // add ruler
  EvilScrolledWindow* scw_ruler = manage(new EvilScrolledWindow(true, false));
  scw_ruler->add(m_pattern_ruler);
  table->attach(*scw_ruler, 1, 5, 1, 2, FILL|EXPAND, FILL);
  
  // add octave labels
  EvilScrolledWindow* scw_octaves = manage(new EvilScrolledWindow(false, true));
  scw_octaves->add(m_octave_label);
  table->attach(*scw_octaves, 0, 1, 2, 3, FILL, FILL|EXPAND);
  
  // add note editor
  EvilScrolledWindow* scw_note = manage(new EvilScrolledWindow);
  scw_note->add(m_ne);
  table->attach(*scw_note, 1, 5, 2, 3, FILL|EXPAND, FILL|EXPAND);
  
  // add CC editor
  EvilScrolledWindow* scw_cc = manage(new EvilScrolledWindow(true, false));
  scw_cc->add(m_cce);
  table->attach(*scw_cc, 1, 5, 4, 5, FILL|EXPAND, FILL);
  
  // add controller combo
  cb_box = manage(new HBox);
  cb_box->set_spacing(5);
  cb_box->pack_start(*manage(new Label("Controller:")), false, false);
  cb_box->pack_start(m_cmb_controller, true, true);
  table->attach(*cb_box, 1, 5, 3, 4, FILL | EXPAND, FILL);
  
  // add scrollbars
  table->attach(*hscroll, 1, 5, 5, 6, FILL | EXPAND, FILL);
  table->attach(*vscroll, 5, 6, 2, 3, FILL, FILL | EXPAND);

  // connect the combo boxes
  m_cmb_track.set_sensitive(false);
  m_cmb_pattern.set_sensitive(false);
  m_cmb_controller.set_sensitive(false);
  m_track_combo_connection = m_cmb_track.signal_changed().
    connect(compose(mem_fun(*this, &PatternEditor::set_active_track),
		    mem_fun(m_cmb_track, &SingleTextCombo::get_active_id)));
  m_pattern_combo_connection = m_cmb_pattern.signal_changed().
    connect(compose(mem_fun(*this, &PatternEditor::set_active_pattern),
		    mem_fun(m_cmb_pattern, &SingleTextCombo::get_active_id)));
  m_cmb_controller.signal_changed().
    connect(compose(mem_fun(*this, &PatternEditor::set_active_controller),
		    mem_fun(m_cmb_controller, &SingleTextCombo::get_active_id)));

  // synchronise scrolling
  scw_ruler->set_hadjustment(scw_note->get_hadjustment());
  hscroll->set_adjustment(*scw_note->get_hadjustment());
  vscroll->set_adjustment(*scw_note->get_vadjustment());
  scw_cc->set_hadjustment(*scw_note->get_hadjustment());
  scw_octaves->set_vadjustment(*scw_note->get_vadjustment());
  m_ne.set_vadjustment(scw_note->get_vadjustment());

  // get the dialogs
  m_dlg_pattern = new PatternDialog;
  m_dlg_controller = new ControllerDialog;
  
  // the toolbuttons
  add_toolbutton(tbar, m_tbn_add_pattern, Stock::ADD, "Create new pattern",
                 &PatternEditor::add_pattern);
  add_toolbutton(tbar, m_tbn_delete_pattern, Stock::DELETE, "Delete pattern",
                 &PatternEditor::delete_pattern);
  add_toolbutton(tbar, m_tbn_duplicate_pattern, Stock::COPY, 
                 "Duplicate pattern", &PatternEditor::duplicate_pattern);
  add_toolbutton(tbar, m_tbn_set_pattern_properties, Stock::PROPERTIES, 
                 "Edit pattern properties", 
                 &PatternEditor::edit_pattern_properties);
  tbar->append(*manage(new SeparatorToolItem));
  add_toolbutton(tbar, m_tbn_add_controller, Stock::ADD, "Add controller", 
                 &PatternEditor::add_controller);
  add_toolbutton(tbar, m_tbn_delete_controller, Stock::DELETE, 
                 "Delete controller", &PatternEditor::delete_controller);
  
  m_tbn_add_pattern->set_sensitive(false);
  m_tbn_delete_pattern->set_sensitive(false);
  m_tbn_duplicate_pattern->set_sensitive(false);
  m_tbn_set_pattern_properties->set_sensitive(false);
  m_tbn_add_controller->set_sensitive(false);
  m_tbn_delete_controller->set_sensitive(false);
  
  // connect to song
  m_pattern_ruler.set_song(&song);
  slot<void> update_t_combo = 
    mem_fun(*this, &PatternEditor::update_track_combo);
  m_song.signal_track_added().connect(sigc::hide(update_t_combo));
  m_song.signal_track_removed().connect(sigc::hide(update_t_combo));
  
  pack_start(*v);
  
  reset_gui();
}


void PatternEditor::reset_gui() {
  update_track_combo();
  update_pattern_combo();
  update_controller_combo();
  slot<void> update_menu = bind(mem_fun(m_ne, &NoteEditor::update_menu), 
				ref(*m_plif));
  m_plif->signal_action_added().connect(sigc::hide(update_menu));
  m_plif->signal_action_removed().connect(sigc::hide(update_menu));
  update_menu();
}


void PatternEditor::cut_selection() {
  m_ne.cut_selection();
}


void PatternEditor::copy_selection() {
  m_ne.copy_selection();
}


void PatternEditor::paste() {
  m_ne.paste();
}


void PatternEditor::delete_selection() {
  m_ne.delete_selection();
}


void PatternEditor::select_all() {
  m_ne.select_all();
}


void PatternEditor::update_track_combo() {
  m_track_combo_connection.block();
  int oldActive = m_cmb_track.get_active_id();
  m_cmb_track.clear();
  int newActive = m_active_track;
  if (m_song.get_number_of_tracks() > 0) {
    char tmp[10];
    Song::TrackIterator iter;
    for (iter = m_song.tracks_begin(); iter != m_song.tracks_end(); ++iter) {
      sprintf(tmp, "%03d ", iter->get_id());
      m_cmb_track.append_text(string(tmp) + iter->get_name(), 
			      iter->get_id());
      if (newActive == -1 || (m_active_track == -1 &&
			      iter->get_id() <= oldActive))
	newActive = iter->get_id();
    }
  }
  else {
    m_cmb_track.append_text("No tracks");
    newActive = -1;
  }
  m_track_combo_connection.unblock();
  m_cmb_track.set_active_id(newActive);
  m_cmb_track.set_sensitive(newActive != -1);
}


void PatternEditor::update_pattern_combo() {
  m_pattern_combo_connection.block();
  int newActive = m_active_pattern;
  m_cmb_pattern.clear();
  int trackID = m_cmb_track.get_active_id();
  if (trackID >= 0) {
    Song::ConstTrackIterator trk = m_song.tracks_find(trackID);
    if (trk->pat_find(newActive) == trk->pat_end())
      newActive = -1;
    Track::ConstPatternIterator iter;
    char tmp[10];
    for (iter = trk->pat_begin(); iter != trk->pat_end(); ++iter) {
      sprintf(tmp, "%03d ", iter->get_id());
      m_cmb_pattern.append_text(string(tmp) + iter->get_name(), 
				iter->get_id());
      if (newActive == -1)
	newActive = iter->get_id();
    }
  }
  
  if (newActive == -1)
    m_cmb_pattern.append_text("No patterns");
  m_cmb_pattern.set_sensitive(newActive != -1);
  m_pattern_combo_connection.unblock();
  m_cmb_pattern.set_active_id(newActive);
}


void PatternEditor::update_controller_combo() {
  m_cmb_controller.clear();
  long new_active = m_active_controller;
  Song::TrackIterator t_iter = m_song.tracks_find(m_active_track);
  if (t_iter != m_song.tracks_end()) {
    Track::PatternIterator p_iter = t_iter->pat_find(m_active_pattern);
    if (p_iter != t_iter->pat_end()) {
      Pattern::ControllerIterator iter;
      if (p_iter->ctrls_find(new_active) == p_iter->ctrls_end())
	new_active = -1;
      char tmp[10];
      for (iter = p_iter->ctrls_begin(); iter != p_iter->ctrls_end(); ++iter) {
	long param = iter->get_param();
	string name = iter->get_name();
	if (is_cc(param))
	  sprintf(tmp, "CC%03ld: ", cc_number(param));
	else if (is_nrpn(param))
	  sprintf(tmp, "NRPN%03ld: ", nrpn_number(param));
	else if (is_pbend(param))
	  tmp[0] = '\0';
	m_cmb_controller.append_text(string(tmp) + name, param);
	if (new_active == -1)
	  new_active = param;
      }
    }
  }
  if (new_active == -1)
    m_cmb_controller.append_text("No controllers");
  m_cmb_controller.set_sensitive(new_active != -1);
  m_cmb_controller.set_active_id(new_active);
}


void PatternEditor::set_active_track(int track) {
  if (track == m_active_track)
    return;

  m_active_track = track;
  
  // update connections
  m_conn_pat_added.disconnect();
  m_conn_pat_removed.disconnect();
  if (m_active_track != -1) {
    Song::TrackIterator t = m_song.tracks_find(m_active_track);
    m_conn_pat_added = t->signal_pattern_added().
      connect(mem_fun(*this, &PatternEditor::pattern_added));
    m_conn_pat_removed = t->signal_pattern_removed().
      connect(sigc::hide(mem_fun(*this, &PatternEditor::update_pattern_combo)));
  }
  set_active_pattern(-1);
  update_pattern_combo();
  
  bool active = (m_active_track != -1);
  m_tbn_add_pattern->set_sensitive(active);
}


void PatternEditor::set_active_pattern(int pattern) {
  if (pattern == m_active_pattern)
    return;
  
  m_active_pattern = pattern;
  Song::TrackIterator t = m_song.tracks_find(m_active_track);
  if (t == m_song.tracks_end())
    return;
  Track::PatternIterator p = t->pat_find(m_active_pattern);
  
  // update connections
  m_conn_cont_added.disconnect();
  m_conn_cont_removed.disconnect();
  Pattern* pptr = 0;
  if (m_active_pattern != -1) {
    slot<void> uslot = mem_fun(*this, &PatternEditor::update_controller_combo);
    m_conn_cont_added = p->signal_controller_added().connect(sigc::hide(uslot));
    m_conn_cont_removed = p->signal_controller_removed().
      connect(sigc::hide(uslot));
    pptr = &*p;
  }

  update_controller_combo();
  m_ne.set_pattern(pptr);
  m_pattern_ruler.set_pattern(m_active_track, m_active_pattern);

  bool active = (m_active_pattern != -1);
  m_tbn_delete_pattern->set_sensitive(active);
  m_tbn_duplicate_pattern->set_sensitive(active);
  m_tbn_set_pattern_properties->set_sensitive(active);
  m_tbn_add_controller->set_sensitive(active);
}


void PatternEditor::set_active_controller(long controller) {
  m_active_controller = controller;
  
  Song::TrackIterator t = m_song.tracks_find(m_active_track);
  if (t == m_song.tracks_end()) {
    m_cce.set_controller(0, make_invalid());
    return;
  }
  
  Track::PatternIterator p = t->pat_find(m_active_pattern);
  if (p == t->pat_end()) {
    m_cce.set_controller(0, make_invalid());
    return;
  }
  
  m_cce.set_controller(&*p, m_active_controller);
  
  bool active = controller_is_set(m_active_controller);
  m_tbn_delete_controller->set_sensitive(active);
}

  
void PatternEditor::add_controller() {
  if (m_active_track >= 0 && m_active_pattern >= 0) {
    m_dlg_controller->set_controller(make_cc(1));
    m_dlg_controller->refocus();
    m_dlg_controller->show_all();
    while (m_dlg_controller->run() == RESPONSE_OK) {
      int min = 0, max = 127;
      long controller = m_dlg_controller->get_controller();
      if (is_pbend(controller)) {
	min = -8192;
	max = 8191;
      }
      Track::PatternIterator iter = m_song.tracks_find(m_active_track)->
	pat_find(m_active_pattern);
      if (iter->ctrls_find(controller) != iter->ctrls_end()) {
	MessageDialog dlg("That controller already exists!", 
			  false, MESSAGE_ERROR);
	dlg.run();
      }
      else {
	iter->add_controller(m_dlg_controller->get_name(), 
			     controller, min, max);
	m_cmb_controller.set_active_id(controller);
	break;
      }
    }
    m_dlg_controller->hide();
  }
}


void PatternEditor::delete_controller() {
  if (m_active_track >= 0 && m_active_pattern >= 0 && 
      m_active_controller >= 0) {
    Pattern* pat = &*m_song.tracks_find(m_active_track)->
      pat_find(m_active_pattern);
    pat->remove_controller(pat->ctrls_find(m_active_controller));
  }
}


void PatternEditor::add_pattern() {
  if (m_active_track >= 0) {
    m_dlg_pattern->set_name("Untitled");
    m_dlg_pattern->set_length(8);
    m_dlg_pattern->set_steps(8);
    m_dlg_pattern->refocus();
    m_dlg_pattern->show_all();
    if (m_dlg_pattern->run() == RESPONSE_OK) {
      Track::PatternIterator iter = m_song.tracks_find(m_active_track)->
	add_pattern(m_dlg_pattern->get_name(),
		    m_dlg_pattern->get_length(),
		    m_dlg_pattern->get_steps());
      m_cmb_pattern.set_active_id(iter->get_id());
    }
    m_dlg_pattern->hide();
  }
}


void PatternEditor::delete_pattern() {
  if (m_active_track < 0 || m_active_pattern < 0)
    return;
  Track& trk = *(m_song.tracks_find(m_active_track));
  trk.remove_pattern(m_active_pattern);
}


void PatternEditor::duplicate_pattern() {
  if (m_active_track < 0 || m_active_pattern < 0)
    return;
  Track& trk = *(m_song.tracks_find(m_active_track));
  trk.duplicate_pattern(trk.pat_find(m_active_pattern));
}


void PatternEditor::edit_pattern_properties() {
  if (m_active_track >= 0 && m_active_pattern >= 0) {
    Song::TrackIterator iter = m_song.tracks_find(m_active_track);
    assert(iter != m_song.tracks_end());
    Track::PatternIterator pat = iter->pat_find(m_active_pattern);
    assert(pat != iter->pat_end());

    m_dlg_pattern->set_name(pat->get_name());
    m_dlg_pattern->set_length(pat->get_length());
    m_dlg_pattern->set_steps(pat->get_steps());
    m_dlg_pattern->refocus();
    m_dlg_pattern->show_all();
    if (m_dlg_pattern->run() == RESPONSE_OK) {
      pat->set_name(m_dlg_pattern->get_name());
      pat->set_length(m_dlg_pattern->get_length());
      pat->set_steps(m_dlg_pattern->get_steps());
    }
    m_dlg_pattern->hide();
  }
}


void PatternEditor::pattern_added(int id) {
  update_pattern_combo();
  Song::TrackIterator iter = m_song.tracks_find(m_active_track);
  if (iter != m_song.tracks_end()) {
    iter->pat_find(id)->signal_name_changed().
      connect(sigc::hide(mem_fun(*this, &PatternEditor::update_pattern_combo)));
  }
}


void PatternEditor::add_toolbutton(Gtk::Toolbar* tbar, 
                                   Gtk::ToolButton*& tbutton, 
                                   Gtk::BuiltinStockID stock, 
                                   const std::string& tip,
                                   void (PatternEditor::*button_slot)()) {
  Image* img = manage(new Image(stock, ICON_SIZE_SMALL_TOOLBAR));
  tbutton = manage(new ToolButton(*img));
  tbutton->set_tooltip(m_tooltips, tip);
  tbar->append(*tbutton, mem_fun(*this, button_slot));
}


