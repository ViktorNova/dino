#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <sstream>
#include <utility>

#include "dino.hpp"
#include "evilscrolledwindow.hpp"
#include "ruler.hpp"
#include "song.hpp"
#include "trackwidget.hpp"


Dino::Dino(int argc, char** argv, RefPtr<Xml> xml) 
  : m_seq("Dino"), m_pattern_ruler_1(0, 1, 1, 20, 20) {
  
  m_seq.set_song(m_song);
  
  // insert the pattern editor widgets into the GUI
  m_window = w<Gtk::Window>(xml, "main_window");
  w<HBox>(xml, "hbx_track_combo")->pack_start(m_cmb_track);
  m_track_pattern_connection = m_cmb_track.signal_changed().
    connect(bind(sigc::mem_fun(*this, &Dino::update_pattern_combo), -1));
  m_pattern_editor_connection = m_cmb_pattern.signal_changed().
    connect(sigc::mem_fun(*this, &Dino::update_editor_widgets));
  w<HBox>(xml, "hbx_pattern_combo")->pack_start(m_cmb_pattern);
  Scrollbar* scbHorizontal = w<Scrollbar>(xml, "scb_pattern_editor");
  Scrollbar* scbVertical = w<Scrollbar>(xml, "scb_note_editor");
  Box* boxPatternRuler1 = w<Box>(xml, "box_pattern_ruler_1");
  EvilScrolledWindow* scwPatternRuler1 = 
    manage(new EvilScrolledWindow(true, false));
  boxPatternRuler1->pack_start(*scwPatternRuler1);
  VBox* vboxtmp = new VBox;
  scwPatternRuler1->add(*vboxtmp);
  vboxtmp->pack_start(m_pattern_ruler_1);
  EvilScrolledWindow* scwNoteEditor = manage(new EvilScrolledWindow);
  scwPatternRuler1->set_hadjustment(scwNoteEditor->get_hadjustment());
  EvilScrolledWindow* scwCCEditor = manage(new EvilScrolledWindow(true,false));
  Box* boxNoteEditor = w<Box>(xml, "box_note_editor");
  Box* boxCCEditor = w<Box>(xml, "box_cc_editor");
  boxNoteEditor->pack_start(*scwNoteEditor);
  boxCCEditor->pack_start(*scwCCEditor);
  scwNoteEditor->add(m_pe);
  scwCCEditor->add(m_cce);
  scbHorizontal->set_adjustment(*scwNoteEditor->get_hadjustment());
  scwCCEditor->set_hadjustment(*scwNoteEditor->get_hadjustment());
  scbVertical->set_adjustment(*scwNoteEditor->get_vadjustment());
  m_sb_cc_number = w<SpinButton>(xml, "sb_cc_number");
  m_lb_cc_description = w<Label>(xml, "lb_cc_description");
  m_sb_cc_number->signal_value_changed().
    connect(sigc::mem_fun(this, &Dino::slot_cc_number_changed));
  m_sb_cc_number->set_value(1);
  m_sb_cc_number->set_numeric(true);
  m_sb_cc_editor_size = w<SpinButton>(xml, "sb_cc_editor_size");
  m_sb_cc_editor_size->signal_value_changed().
    connect(sigc::mem_fun(this, &Dino::slot_cc_editor_size_changed));
  m_sb_cc_editor_size->set_editable(false);
  
  // insert the arrangement editor widgets into the GUI
  VBox* boxArrangementEditor = w<VBox>(xml, "box_arrangement_editor");
  scbHorizontal = w<Scrollbar>(xml, "scbh_arrangement_editor");
  scbVertical = w<Scrollbar>(xml, "scbv_arrangement_editor");
  Box* boxSequenceRuler = w<Box>(xml, "box_sequence_ruler");
  EvilScrolledWindow* scwSequenceRuler = 
    manage(new EvilScrolledWindow(true, false));
  boxSequenceRuler->pack_start(*scwSequenceRuler);
  vboxtmp = new VBox;
  scwSequenceRuler->add(*vboxtmp);
  vboxtmp->pack_start(*manage(new ::Ruler(m_song.get_length(), 1, 4, 20, 20)));
  EvilScrolledWindow* scwArrangementEditor = manage(new EvilScrolledWindow);
  scwSequenceRuler->set_hadjustment(scwArrangementEditor->get_hadjustment());
  scbHorizontal->set_adjustment(*scwArrangementEditor->get_hadjustment());
  scbVertical->set_adjustment(*scwArrangementEditor->get_vadjustment());
  boxArrangementEditor->pack_start(*scwArrangementEditor);
  m_vbx_track_editor = manage(new VBox(false, 2));
  scwArrangementEditor->add(*m_vbx_track_editor);
  update_track_widgets();
  update_track_combo();
  
  // get other widgets
  m_about_dialog = w<Dialog>(xml, "dlg_about");
  
  // connect menu signals
  map<const char*, void (Dino::*)(void)> menuSlots;
  menuSlots["new1"] = &Dino::slot_file_new;
  menuSlots["open1"] = &Dino::slot_file_open;
  menuSlots["save1"] = &Dino::slot_file_save;
  menuSlots["save_as1"] = &Dino::slot_file_save_as;
  menuSlots["quit1"] = &Dino::slot_file_quit;
  menuSlots["cut1"] = &Dino::slot_edit_cut;
  menuSlots["copy1"] = &Dino::slot_edit_copy;
  menuSlots["paste1"] = &Dino::slot_edit_paste;
  menuSlots["delete1"] = &Dino::slot_edit_delete;
  menuSlots["add_track1"] = &Dino::slot_edit_add_track;
  menuSlots["delete_track1"] = &Dino::slot_edit_delete_track;
  menuSlots["add_pattern1"] = &Dino::slot_edit_add_pattern;
  menuSlots["delete_pattern1"] = &Dino::slot_edit_delete_pattern;
  menuSlots["about1"] = &Dino::slot_help_about_dino;
  menuSlots["play1"] = &Dino::slot_transport_play;
  menuSlots["stop1"] = &Dino::slot_transport_stop;
  menuSlots["go_to_start1"] = &Dino::slot_transport_go_to_start;
  map<const char*, void (Dino::*)(void)>::const_iterator iter;
  for (iter = menuSlots.begin(); iter != menuSlots.end(); ++iter) {
    MenuItem* mi = w<Gtk::MenuItem>(xml, iter->first);
    assert(mi);
    mi->signal_activate().connect(sigc::mem_fun(*this, iter->second));
  }
  
  m_window->show_all();
}


Gtk::Window* Dino::get_window() {
  return m_window;
}


void Dino::slot_file_new() {
  m_song = Song();
}


void Dino::slot_file_open() {
  
}


void Dino::slot_file_save() {
  m_song.write_file("output.dino");
}


void Dino::slot_file_save_as() {
  m_song.write_file("output.dino");
}


void Dino::slot_file_quit() {
  Main::quit();
}


void Dino::slot_edit_cut() {

}


void Dino::slot_edit_copy() {

}


void Dino::slot_edit_paste() {

}


void Dino::slot_edit_delete() {
  
}


void Dino::slot_edit_add_track() {
  m_song.add_track();
  update_track_widgets();
  update_track_combo();
}
 

void Dino::slot_edit_delete_track() {
  int trackID = m_cmb_track.get_active_id();
  if (trackID >= 0) {
    m_song.remove_track(trackID);
    update_track_widgets();
    update_track_combo();
  }
}


void Dino::slot_edit_add_pattern() {
  int trackID = m_cmb_track.get_active_id();
  if (trackID >= 0) {
    int patternID = m_song.get_tracks().find(trackID)->second.add_pattern(8,4, 4);
    Pattern* pat = &m_song.get_tracks().find(trackID)->
      second.get_patterns().find(patternID)->second;
    update_pattern_combo(patternID);
  }
}
 

void Dino::slot_edit_delete_pattern() {
  
}


void Dino::slot_transport_play() {
  m_seq.play();
}


void Dino::slot_transport_stop() {
  m_seq.stop();
}


void Dino::slot_transport_go_to_start() {
  m_seq.goto_beat(0);
}


void Dino::slot_help_about_dino() {
  assert(m_about_dialog != NULL);
  m_about_dialog->run();
}


void Dino::update_track_widgets() {
  m_vbx_track_editor->children().clear();
  for (map<int, Track>::iterator iter = m_song.get_tracks().begin();
       iter != m_song.get_tracks().end(); ++iter) {
    TrackWidget* tw = manage(new TrackWidget(&m_song));
    tw->set_track(&iter->second);
    m_vbx_track_editor->pack_start(*tw, PACK_SHRINK);
  }
  m_vbx_track_editor->show_all();
}


void Dino::update_track_combo() {
  m_track_pattern_connection.block();
  int oldActive = m_cmb_track.get_active_id();
  m_cmb_track.clear();
  int newActive = -1;
  if (m_song.get_tracks().size() > 0) {
    char tmp[10];
    for (map<int, Track>::iterator iter = m_song.get_tracks().begin();
	 iter != m_song.get_tracks().end(); ++iter) {
      sprintf(tmp, "%03d", iter->first);
      m_cmb_track.append_text(tmp, iter->first);
      if (newActive == -1 || iter->first <= oldActive)
	newActive = iter->first;
    }
  }
  else {
    m_cmb_track.append_text("No tracks");
  }
  m_cmb_track.set_active_id(newActive);
  m_track_pattern_connection.unblock();
  if (oldActive == -1 || newActive != oldActive)
    update_pattern_combo();
}


void Dino::update_pattern_combo(int activePattern) {
  int newActive = activePattern;
  m_cmb_pattern.clear();
  int trackID = m_cmb_track.get_active_id();
  if (trackID >= 0) {
    const Track& trk(m_song.get_tracks().find(trackID)->second);
    m_cmb_pattern.clear();
    map<int, Pattern>::const_iterator iter;
    char tmp[10];
    for (iter = trk.get_patterns().begin(); 
	 iter != trk.get_patterns().end(); ++iter) {
      sprintf(tmp, "%03d", iter->first);
      m_cmb_pattern.append_text(tmp, iter->first);
      if (newActive == -1)
	newActive = iter->first;
    }
  }
  
  if (newActive == -1)
    m_cmb_pattern.append_text("No patterns");
  m_cmb_pattern.set_active_id(newActive);
  update_editor_widgets();
}


void Dino::update_editor_widgets() {
  int trackID = m_cmb_track.get_active_id();
  int patternID = m_cmb_pattern.get_active_id();
  Pattern* pat = NULL;
  if (trackID != -1 && patternID != -1) {
    Track& trk = m_song.get_tracks().find(trackID)->second;
    pat = &(trk.get_patterns().find(patternID)->second);
  }
  m_pe.set_pattern(pat);
  m_cce.set_pattern(pat);
  if (pat) {
    m_pattern_ruler_1.setLength(pat->get_length());
    m_pattern_ruler_1.setSubdivisions(pat->get_steps());
    m_pattern_ruler_1.setDivisionSize(8 * pat->get_steps());
  }
  else {
    m_pattern_ruler_1.setLength(0);
  }
}


void Dino::slot_cc_number_changed() {
  m_lb_cc_description->set_text(cc_descriptions[m_sb_cc_number->get_value_as_int()]);
  m_cce.set_cc_number(m_sb_cc_number->get_value_as_int());
}


void Dino::slot_cc_editor_size_changed() {
  m_cce.set_height(m_sb_cc_editor_size->get_value_as_int());
}


char* Dino::cc_descriptions[] = { "Bank select MSB",
				  "Modulation", 
				  "Breath controller",
				  "Undefined",
				  "Foot controller",
				  "Portamento time",
				  "Data entry MSB",
				  "Channel volume",
				  "Balance", 
				  "Undefined",
				  "Pan",
				  "Expression",
				  "Effect control 1",
				  "Effect control 2",
				  "Undefined",
				  "Undefined",
				  "General purpose controller 1",
				  "General purpose controller 2",
				  "General purpose controller 3",
				  "General purpose controller 4",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Bank select LSB",
				  "Modulation LSB",
				  "Breath controller LSB",
				  "Undefined 03 LSB",
				  "Foot controller LSB",
				  "Portamento time LSB",
				  "Data entry LSB",
				  "Channel volume LSB",
				  "Balance LSB",
				  "Undefined 09 LSB",
				  "Pan LSB",
				  "Expression LSB",
				  "Effect control 1 LSB",
				  "Effect control 2 LSB",
				  "Undefined 14 LSB",
				  "Undefined 15 LSB",
				  "General purpose controller 1 LSB",
				  "General purpose controller 2 LSB",
				  "General purpose controller 3 LSB",
				  "General purpose controller 4 LSB",
				  "Undefined 20 LSB",
				  "Undefined 21 LSB",
				  "Undefined 22 LSB",
				  "Undefined 23 LSB",
				  "Undefined 24 LSB",
				  "Undefined 25 LSB",
				  "Undefined 26 LSB",
				  "Undefined 27 LSB",
				  "Undefined 28 LSB",
				  "Undefined 29 LSB",
				  "Undefined 30 LSB",
				  "Undefined 31 LSB",
				  "Damper pedal on/off",
				  "Portamento on/off",
				  "Sustenuto on/off",
				  "Soft pedal on/off",
				  "Legato on/off",
				  "Hold 2 on/off",
				  "Sound controller 1 - Variation",
				  "Sound controller 2 - Timbre",
				  "Sound controller 3 - Release time",
				  "Sound controller 4 - Attack time",
				  "Sound controller 5 - Brightness",
				  "Sound controller 6 - Decay time",
				  "Sound controller 7 - Vibrato rate",
				  "Sound controller 8 - Vibrato depth",
				  "Sound controller 9 - Vibrato delay",
				  "Sound controller 10",
				  "General purpose controller 5",
				  "General purpose controller 6",
				  "General purpose controller 7",
				  "General purpose controller 8",
				  "Portamento control",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Effect depth 1 - Reverb",
				  "Effect depth 2 - Tremolo",
				  "Effect depth 3 - Chorus",
				  "Effect depth 4 - Celeste",
				  "Effect depth 5 - Phaser",
				  "Data increment",
				  "Data decrement",
				  "NRPN LSB",
				  "NRPN MSB",
				  "RPN LSB",
				  "RPN MSB",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "Undefined",
				  "CMM All sound off",
				  "CMM Reset all controllers",
				  "CMM Local control on/off",
				  "CMM All notes off",
				  "CMM Omni mode off",
				  "CMM Omni mode on",
				  "CMM Poly mode on/off",
				  "CMM Poly mode on",
				  NULL };

				  
