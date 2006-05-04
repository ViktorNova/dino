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

#include "controllerdialog.hpp"
#include "controller_numbers.hpp"
#include "evilscrolledwindow.hpp"
#include "patterndialog.hpp"
#include "patterneditor.hpp"
#include "song.hpp"
#include "track.hpp"


using namespace Dino;
using namespace Glib;
using namespace Gnome::Glade;
using namespace Gtk;
using namespace sigc;


PatternEditor::PatternEditor(BaseObjectType* cobject, 
				     const Glib::RefPtr<Gnome::Glade::Xml>& xml)
  : Gtk::VBox(cobject),
    m_octave_label(20, 8),
    m_active_track(-1),
    m_active_pattern(-1),
    m_active_controller(-1),
    m_song(0) {

  // get all the widgets from the glade file
  Box* boxPatternRuler1 = w<Box>(xml, "box_pattern_ruler_1");
  HBox* hbx_track_combo = w<HBox>(xml, "hbx_track_combo");
  HBox* hbx_pattern_combo = w<HBox>(xml, "hbx_pattern_combo");
  HBox* hbx_controller_combo = w<HBox>(xml, "hbx_controller_combo");
  Scrollbar* scbHorizontal = w<Scrollbar>(xml, "scb_pattern_editor");
  Scrollbar* scbVertical = w<Scrollbar>(xml, "scb_note_editor");
  Box* boxNoteEditor = w<Box>(xml, "box_note_editor");
  Box* boxCCEditor = w<Box>(xml, "box_cc_editor");
  Box* box_octave_label = w<Box>(xml, "box_octave_label");
  
  // add and connect the combo boxes
  hbx_pattern_combo->pack_start(m_cmb_pattern);
  hbx_track_combo->pack_start(m_cmb_track);
  hbx_controller_combo->pack_start(m_cmb_controller);
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
		    mem_fun(m_cmb_controller,&SingleTextCombo::get_active_id)));
  
  // add the ruler
  EvilScrolledWindow* scwPatternRuler1 = 
    manage(new EvilScrolledWindow(true, false));
  boxPatternRuler1->pack_start(*scwPatternRuler1);
  scwPatternRuler1->add(m_pattern_ruler);
  
  // add the note editor
  EvilScrolledWindow* scwNoteEditor = manage(new EvilScrolledWindow);
  boxNoteEditor->pack_start(*scwNoteEditor);
  scwNoteEditor->add(m_ne);
  
  // add the octave labels
  EvilScrolledWindow* scwOctaveLabel = 
    manage(new EvilScrolledWindow(false, true));
  box_octave_label->pack_start(*scwOctaveLabel);
  scwOctaveLabel->add(m_octave_label);
  
  // add the CC editor
  EvilScrolledWindow* scwCCEditor = manage(new EvilScrolledWindow(true,false));
  boxCCEditor->pack_start(*scwCCEditor);
  scwCCEditor->add(m_cce);
  
  // synchronise scrolling
  scwPatternRuler1->set_hadjustment(scwNoteEditor->get_hadjustment());
  scbHorizontal->set_adjustment(*scwNoteEditor->get_hadjustment());
  scbVertical->set_adjustment(*scwNoteEditor->get_vadjustment());
  scwCCEditor->set_hadjustment(*scwNoteEditor->get_hadjustment());
  scwOctaveLabel->set_vadjustment(*scwNoteEditor->get_vadjustment());
  m_ne.set_vadjustment(scwNoteEditor->get_vadjustment());

  // get the dialogs
  m_dlg_pattern = xml->get_widget_derived("dlg_pattern_properties", 
					  m_dlg_pattern);
  m_dlg_controller = xml->get_widget_derived("dlg_controller_properties",
					     m_dlg_controller);
  
  // the toolbuttons
  std::map<string, void (PatternEditor::*)(void)> toolSlots;
  toolSlots["tbn_add_controller"] = &PatternEditor::add_controller;
  toolSlots["tbn_delete_controller"] = &PatternEditor::delete_controller;
  toolSlots["tbn_add_pattern"] = &PatternEditor::add_pattern;
  toolSlots["tbn_delete_pattern"] = &PatternEditor::delete_pattern;
  toolSlots["tbn_duplicate_pattern"] = &PatternEditor::duplicate_pattern;
  toolSlots["tbn_set_pattern_properties"] = 
    &PatternEditor::edit_pattern_properties;
  std::map<string, void (PatternEditor::*)(void)>::const_iterator iter;
  for (iter = toolSlots.begin(); iter != toolSlots.end(); ++iter) {
    m_toolbuttons[iter->first] = w<ToolButton>(xml, iter->first);
    m_toolbuttons[iter->first]->signal_clicked().
      connect(mem_fun(*this, iter->second));
    m_toolbuttons[iter->first]->set_sensitive(false);
  }

}


void PatternEditor::set_song(Song* song) {
  m_song = song;
  
  m_pattern_ruler.set_song(song);
  slot<void> update_t_combo = 
    mem_fun(*this, &PatternEditor::update_track_combo);
  m_song->signal_track_added.connect(sigc::hide(update_t_combo));
  m_song->signal_track_removed.connect(sigc::hide(update_t_combo));
}


void PatternEditor::reset_gui() {
  update_track_combo();
  update_pattern_combo();
  update_controller_combo();
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
  if (m_song->get_number_of_tracks() > 0) {
    char tmp[10];
    Song::TrackIterator iter;
    for (iter = m_song->tracks_begin(); iter != m_song->tracks_end(); ++iter) {
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
    Song::ConstTrackIterator trk = m_song->tracks_find(trackID);
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
  Song::TrackIterator t_iter = m_song->tracks_find(m_active_track);
  if (t_iter != m_song->tracks_end()) {
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
    Song::TrackIterator t = m_song->tracks_find(m_active_track);
    m_conn_pat_added = t->signal_pattern_added.
      connect(mem_fun(*this, &PatternEditor::pattern_added));
    m_conn_pat_removed = t->signal_pattern_removed.
      connect(sigc::hide(mem_fun(*this, &PatternEditor::update_pattern_combo)));
  }
  set_active_pattern(-1);
  update_pattern_combo();
  
  bool active = (m_active_track != -1);
  m_toolbuttons["tbn_add_pattern"]->set_sensitive(active);
}


void PatternEditor::set_active_pattern(int pattern) {
  if (pattern == m_active_pattern)
    return;
  
  Song::TrackIterator t = m_song->tracks_find(m_active_track);
  if (t == m_song->tracks_end())
    return;
  
  Track::PatternIterator p = t->pat_find(m_active_pattern);
  
  // update connections
  m_conn_cont_added.disconnect();
  m_conn_cont_removed.disconnect();
  if (m_active_pattern != -1) {
    slot<void> uslot = 
      mem_fun(*this, &PatternEditor::update_controller_combo);
    m_conn_cont_added = p->signal_controller_added.connect(sigc::hide(uslot));
    m_conn_cont_removed=p->signal_controller_removed.connect(sigc::hide(uslot));
  }

  update_controller_combo();
  m_ne.set_pattern(&*p);
  m_pattern_ruler.set_pattern(m_active_track, m_active_pattern);

  bool active = (m_active_pattern != -1);
  m_toolbuttons["tbn_delete_pattern"]->set_sensitive(active);
  m_toolbuttons["tbn_duplicate_pattern"]->set_sensitive(active);
  m_toolbuttons["tbn_set_pattern_properties"]->set_sensitive(active);
  m_toolbuttons["tbn_add_controller"]->set_sensitive(active);
}


void PatternEditor::set_active_controller(long controller) {
  m_active_controller = controller;
  
  Song::TrackIterator t = m_song->tracks_find(m_active_track);
  if (t == m_song->tracks_end()) {
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
  m_toolbuttons["tbn_delete_controller"]->set_sensitive(active);
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
      Track::PatternIterator iter = m_song->tracks_find(m_active_track)->
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
    Pattern* pat = &*m_song->tracks_find(m_active_track)->
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
      Track::PatternIterator iter = m_song->tracks_find(m_active_track)->
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
  Track& trk = *(m_song->tracks_find(m_active_track));
  trk.remove_pattern(m_active_pattern);
}


void PatternEditor::duplicate_pattern() {
  if (m_active_track < 0 || m_active_pattern < 0)
    return;
  Track& trk = *(m_song->tracks_find(m_active_track));
  trk.duplicate_pattern(trk.pat_find(m_active_pattern));
}


void PatternEditor::edit_pattern_properties() {
  if (m_active_track >= 0 && m_active_pattern >= 0) {
    Song::TrackIterator iter = m_song->tracks_find(m_active_track);
    assert(iter != m_song->tracks_end());
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
  Song::TrackIterator iter = m_song->tracks_find(m_active_track);
  if (iter != m_song->tracks_end()) {
    iter->pat_find(id)->signal_name_changed.
      connect(sigc::hide(mem_fun(*this, &PatternEditor::update_pattern_combo)));
  }
}
