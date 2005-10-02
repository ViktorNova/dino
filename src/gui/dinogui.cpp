#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <sstream>
#include <utility>

#include "deleter.hpp"
#include "dinogui.hpp"
#include "evilscrolledwindow.hpp"
#include "pattern.hpp"
#include "ruler.hpp"
#include "song.hpp"
#include "tempolabel.hpp"
#include "tempowidget.hpp"
#include "track.hpp"
#include "tracklabel.hpp"
#include "trackwidget.hpp"


using namespace sigc;



DinoGUI::DinoGUI(int argc, char** argv, RefPtr<Xml> xml) 
  : m_active_track(-1), m_active_pattern(-1), m_active_controller(-1),
    m_xml(xml), m_sequence_ruler(32, 1, 4, 20, 20), m_pattern_ruler_1(m_song), 
    m_octave_label(20, 8), m_seq("Dino", m_song) {
  
  init_lash(argc, argv);
  
  m_window = w<Gtk::Window>("main_window");
  m_about_dialog = w<Dialog>("dlg_about");
  
  init_pattern_editor();
  init_sequence_editor();
  init_info_editor();
  init_menus();
  reset_gui();
  
  m_window->show_all();
}


Gtk::Window* DinoGUI::get_window() {
  return m_window;
}


void DinoGUI::slot_file_new() {
  //m_song = Song();
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


void DinoGUI::slot_file_quit() {
  Main::quit();
}


void DinoGUI::slot_edit_cut() {

}


void DinoGUI::slot_edit_copy() {

}


void DinoGUI::slot_edit_paste() {

}


void DinoGUI::slot_edit_delete() {
  
}


void DinoGUI::slot_edit_add_track() {
  m_dlgtrack_ent_name->set_text("Untitled");
  update_port_combo();
  m_dlgtrack_sbn_channel->set_value(1);
  m_dlgtrack_cmb_port.set_active_id(-1);
  m_dlg_track_properties->show_all();
  if (m_dlg_track_properties->run() == RESPONSE_OK) {
    int id = m_song.add_track(m_dlgtrack_ent_name->get_text());
    m_song.get_track(id)->
      set_channel(m_dlgtrack_sbn_channel->get_value_as_int() - 1);
    int instrument = m_dlgtrack_cmb_port.get_active_id();
    if (instrument == -1)
      m_seq.set_instrument(id, "None");
    else
      m_seq.set_instrument(id, m_dlgtrack_cmb_port.get_active_text());
    set_active_track(id);
  }
  m_dlg_track_properties->hide();
}
 

void DinoGUI::slot_edit_delete_track() {
  if (m_active_track >= 0) {
    m_song.remove_track(m_active_track);
  }
}


void DinoGUI::slot_edit_edit_track_properties() {
  if (m_active_track >= 0) {
    Track* t = m_song.get_track(m_active_track);
    m_dlgtrack_ent_name->set_text(t->get_name());
    update_port_combo();
    m_dlgtrack_sbn_channel->set_value(t->get_channel() + 1);
    m_dlg_track_properties->show_all();
    if (m_dlg_track_properties->run() == RESPONSE_OK) {
      t->set_name(m_dlgtrack_ent_name->get_text());
      t->set_channel(m_dlgtrack_sbn_channel->get_value_as_int() - 1);
      int instrument = m_dlgtrack_cmb_port.get_active_id();
      if (instrument == -1)
	m_seq.set_instrument(m_active_track, "None");
      else
	m_seq.set_instrument(m_active_track, 
			     m_dlgtrack_cmb_port.get_active_text());
    }
  }
  m_dlg_track_properties->hide();
}


void DinoGUI::slot_edit_add_pattern() {
  if (m_active_track >= 0) {
    m_dlgpat_ent_name->set_text("Untitled");
    m_dlgpat_sbn_length->set_value(4);
    m_dlgpat_sbn_steps->set_value(4);
    m_dlgpat_sbn_cc_steps->set_value(1);
    m_dlg_pattern_properties->show_all();
    if (m_dlg_pattern_properties->run() == RESPONSE_OK) {
      m_song.get_track(m_active_track)->
	add_pattern(m_dlgpat_ent_name->get_text(),
		    m_dlgpat_sbn_length->get_value_as_int(),
		    m_dlgpat_sbn_steps->get_value_as_int(),
		    m_dlgpat_sbn_steps->get_value_as_int() *
		    m_dlgpat_sbn_cc_steps->get_value_as_int());
    }
    m_dlg_pattern_properties->hide();
  }
}
 

void DinoGUI::slot_edit_delete_pattern() {
  if (m_active_track < 0 || m_active_pattern < 0)
    return;
  Track* trk = m_song.get_track(m_active_track);
  trk->remove_pattern(m_active_pattern);
}


void DinoGUI::slot_edit_edit_pattern_properties() {
  if (m_active_track >= 0 && m_active_pattern >= 0) {
    Track* trk = m_song.get_track(m_active_track);
    assert(trk != NULL);
    Pattern* pat = trk->get_pattern(m_active_pattern);
    assert(pat != NULL);

    m_dlgpat_ent_name->set_text(pat->get_name());
    m_dlgpat_sbn_length->set_value(pat->get_length());
    m_dlgpat_sbn_steps->set_value(pat->get_steps());
    m_dlgpat_sbn_cc_steps->set_value(pat->get_cc_steps() / pat->get_steps());

    m_dlg_pattern_properties->show_all();
    if (m_dlg_pattern_properties->run() == RESPONSE_OK) {
      // XXX actually change the pattern properties here
      pat->set_name(m_dlgpat_ent_name->get_text());
    }
    m_dlg_pattern_properties->hide();
  }
}


void DinoGUI::slot_edit_add_controller() {
  if (m_active_track >= 0 && m_active_pattern >= 0) {
    Track* trk = m_song.get_track(m_active_track);
    assert(trk != NULL);
    Pattern* pat = trk->get_pattern(m_active_pattern);
    assert(pat != NULL);

    if (pat != NULL) {
      m_dlgcont_ent_name->set_text("Untitled");
      m_dlgcont_cmb_controller.clear();
      
      static char tmp[512];
      int selected = -1;
      
      for (int i = 0; cc_descriptions[i] != NULL; ++i) {
	if (pat->get_controllers().count(i) == 0) {
	  if (selected == -1)
	    selected = i;
	  sprintf(tmp, "%03d ", i);
	  m_dlgcont_cmb_controller.
	    append_text(string(tmp) + cc_descriptions[i], i);
	}
      }
      
      m_dlgcont_cmb_controller.set_active_id(selected);
      if (m_dlg_controller_properties->run() == RESPONSE_OK) {
	pat->add_controller(m_dlgcont_cmb_controller.get_active_id(),
			    m_dlgcont_ent_name->get_text());
      }
      m_dlg_controller_properties->hide();
      
    }
  }
}


void DinoGUI::slot_edit_delete_controller() {
  // XXX this must be implemented
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
  assert(m_about_dialog != NULL);
  m_about_dialog->run();
  m_about_dialog->hide();
}


void DinoGUI::reset_gui() {
  m_active_track = -1;
  m_active_pattern = -1;
  m_active_controller = -1;
  signal_active_track_changed(m_active_track);
  signal_active_pattern_changed(m_active_track, m_active_pattern);
  signal_active_controller_changed(m_active_controller);
  update_track_combo();
  update_pattern_combo();
  update_controller_combo();
  update_editor_widgets();
  update_track_widgets();
  m_ent_title->set_text(m_song.get_title());
  m_ent_author->set_text(m_song.get_author());
}


void DinoGUI::update_track_widgets() {
  m_vbx_track_editor->children().clear();
  m_vbx_track_labels->children().clear();
  TempoWidget* tmpw = manage(new TempoWidget(&m_song));
  m_vbx_track_editor->pack_start(*tmpw, PACK_SHRINK);
  TempoLabel* tmpl = manage(new TempoLabel(&m_song));
  m_vbx_track_labels->pack_start(*tmpl, PACK_SHRINK);
  for (map<int, Track*>::iterator iter = m_song.get_tracks().begin();
       iter != m_song.get_tracks().end(); ++iter) {
    TrackWidget* tw = manage(new TrackWidget(&m_song));
    tw->set_track(iter->second);
    tw->signal_clicked.
      connect(hide(bind(mem_fun(*this, &DinoGUI::set_active_track), iter->first)));
    m_seq.signal_beat_changed.
      connect(mem_fun(*tw, &TrackWidget::set_current_beat));
    m_vbx_track_editor->pack_start(*tw, PACK_SHRINK);
    TrackLabel* tl = manage(new TrackLabel(&m_song));
    tl->set_track(iter->first, iter->second);
    tl->signal_clicked.
      connect(hide(bind(mem_fun(*this, &DinoGUI::set_active_track), iter->first)));
    m_vbx_track_labels->pack_start(*tl, PACK_SHRINK);
    signal_active_track_changed.
      connect(mem_fun(*tl, &TrackLabel::set_active_track));
  }
  m_vbx_track_editor->show_all();
  m_vbx_track_labels->show_all();
}


void DinoGUI::update_track_combo() {
  m_track_combo_connection.block();
  int oldActive = m_cmb_track.get_active_id();
  m_cmb_track.clear();
  int newActive = m_active_track;
  if (m_song.get_tracks().size() > 0) {
    char tmp[10];
    for (map<int, Track*>::iterator iter = m_song.get_tracks().begin();
	 iter != m_song.get_tracks().end(); ++iter) {
      sprintf(tmp, "%03d ", iter->first);
      m_cmb_track.append_text(string(tmp) + iter->second->get_name(), 
			      iter->first);
      if (newActive == -1 || (m_active_track == -1 &&iter->first <= oldActive))
	newActive = iter->first;
    }
  }
  else {
    m_cmb_track.append_text("No tracks");
  }
  m_track_combo_connection.unblock();
  m_cmb_track.set_active_id(newActive);
}


void DinoGUI::update_pattern_combo() {
  m_pattern_combo_connection.block();
  int newActive = m_active_pattern;
  m_cmb_pattern.clear();
  int trackID = m_cmb_track.get_active_id();
  if (trackID >= 0) {
    const Track* trk = m_song.get_track(trackID);
    m_cmb_pattern.clear();
    map<int, Pattern*>::const_iterator iter;
    char tmp[10];
    for (iter = trk->get_patterns().begin(); 
	 iter != trk->get_patterns().end(); ++iter) {
      sprintf(tmp, "%03d ", iter->first);
      m_cmb_pattern.append_text(string(tmp) + iter->second->get_name(), 
				iter->first);
      if (newActive == -1)
	newActive = iter->first;
    }
  }
  
  if (newActive == -1)
    m_cmb_pattern.append_text("No patterns");
  m_pattern_combo_connection.unblock();
  m_cmb_pattern.set_active_id(newActive);
}


void DinoGUI::update_controller_combo() {
  m_cmb_controller.clear();
  int new_active = m_active_controller;
  if (m_active_track >= 0 && m_active_pattern >= 0) {
    Track* trk = m_song.get_track(m_active_track);
    const Pattern* pat = trk->get_pattern(m_active_pattern);
    const map<int, Pattern::EventList>& conts = pat->get_controllers();
    map<int, Pattern::EventList>::const_iterator iter;
    char tmp[10];
    for (iter = conts.begin(); iter != conts.end(); ++iter) {
      sprintf(tmp, "%03d", iter->first);
      m_cmb_controller.append_text(tmp, iter->first);
      if (new_active == -1)
	new_active = iter->first;
    }
  }
  if (new_active == -1)
    m_cmb_controller.append_text("No controllers");
  m_cmb_controller.set_active_id(new_active);
}


void DinoGUI::update_editor_widgets() {

}


void DinoGUI::update_port_combo() {
  m_dlgtrack_cmb_port.clear();
  m_dlgtrack_cmb_port.append_text("None", -1);
  vector<Sequencer::InstrumentInfo> instruments = 
    m_seq.get_instruments(m_active_track);
  int connected = -1;
  for (size_t i = 0; i < instruments.size(); ++i) {
    m_dlgtrack_cmb_port.append_text(instruments[i].name, i);
    if (instruments[i].connected)
      connected = i;
  }
  m_dlgtrack_cmb_port.set_active_id(connected);
}


void DinoGUI::slot_cc_number_changed() {
  //m_lb_cc_description->set_text(cc_descriptions[m_sb_cc_number->get_value_as_int()]);
  m_cce.set_cc_number(m_active_controller);
}


void DinoGUI::slot_cc_editor_size_changed() {
  m_cce.set_height(m_sb_cc_editor_size->get_value_as_int());
}


void DinoGUI::init_pattern_editor() {
  
  // get all the widgets from the glade file
  Box* boxPatternRuler1 = w<Box>("box_pattern_ruler_1");
  HBox* hbx_track_combo = w<HBox>("hbx_track_combo");
  HBox* hbx_pattern_combo = w<HBox>("hbx_pattern_combo");
  HBox* hbx_controller_combo = w<HBox>("hbx_controller_combo");
  Scrollbar* scbHorizontal = w<Scrollbar>("scb_pattern_editor");
  Scrollbar* scbVertical = w<Scrollbar>("scb_note_editor");
  Box* boxNoteEditor = w<Box>("box_note_editor");
  Box* boxCCEditor = w<Box>("box_cc_editor");
  Box* box_octave_label = w<Box>("box_octave_label");
  //m_sb_cc_number = w<SpinButton>("sb_cc_number");
  //m_lb_cc_description = w<Label>("lb_cc_description");
  m_sb_cc_editor_size = w<SpinButton>("sb_cc_editor_size");
  
  // add and connect the combo boxes
  hbx_pattern_combo->pack_start(m_cmb_pattern);
  hbx_track_combo->pack_start(m_cmb_track);
  hbx_controller_combo->pack_start(m_cmb_controller);
  slot<void> update_tracks = mem_fun(*this, &DinoGUI::update_track_combo);
  slot<void> update_patterns = mem_fun(*this, &DinoGUI::update_pattern_combo);
  slot<void> update_controllers = mem_fun(*this, 
					  &DinoGUI::update_controller_combo);
  signal_active_track_changed.
    connect(hide_return(mem_fun(m_cmb_track,&SingleTextCombo::set_active_id)));
  signal_active_track_changed.connect(hide(update_patterns));
  signal_active_pattern_changed.
    connect(hide_return(hide<0>(mem_fun(m_cmb_pattern, 
					&SingleTextCombo::set_active_id))));
  //signal_active_pattern_changed.connect(hide(hide(update_controllers)));
  
  m_track_combo_connection = m_cmb_track.signal_changed().
    connect(compose(mem_fun(*this, &DinoGUI::set_active_track),
		    mem_fun(m_cmb_track, &SingleTextCombo::get_active_id)));
  m_pattern_combo_connection = m_cmb_pattern.signal_changed().
    connect(compose(mem_fun(*this, &DinoGUI::set_active_pattern),
		    mem_fun(m_cmb_pattern, &SingleTextCombo::get_active_id)));
  m_cmb_controller.signal_changed().
    connect(compose(mem_fun(*this, &DinoGUI::set_active_controller),
		    mem_fun(m_cmb_controller, &SingleTextCombo::get_active_id)));
  
  // add the ruler
  EvilScrolledWindow* scwPatternRuler1 = 
    manage(new EvilScrolledWindow(true, false));
  boxPatternRuler1->pack_start(*scwPatternRuler1);
  scwPatternRuler1->add(m_pattern_ruler_1);
  signal_active_pattern_changed.
    connect(mem_fun(m_pattern_ruler_1, &PatternRuler::set_pattern));
  
  // add the note editor
  EvilScrolledWindow* scwNoteEditor = manage(new EvilScrolledWindow);
  boxNoteEditor->pack_start(*scwNoteEditor);
  scwNoteEditor->add(m_pe);
  
  // add the octave labels
  EvilScrolledWindow* scwOctaveLabel = 
    manage(new EvilScrolledWindow(false, true));
  box_octave_label->pack_start(*scwOctaveLabel);
  scwOctaveLabel->add(m_octave_label);
  
  // add the CC editor
  EvilScrolledWindow* scwCCEditor = manage(new EvilScrolledWindow(true,false));
  boxCCEditor->pack_start(*scwCCEditor);
  scwCCEditor->add(m_cce);
  //signal_active_pattern_changed.
  // connect(mem_fun(m_cce, &CCEditor::set_pattern));
  
  // synchronise scrolling
  scwPatternRuler1->set_hadjustment(scwNoteEditor->get_hadjustment());
  scbHorizontal->set_adjustment(*scwNoteEditor->get_hadjustment());
  scbVertical->set_adjustment(*scwNoteEditor->get_vadjustment());
  scwCCEditor->set_hadjustment(*scwNoteEditor->get_hadjustment());
  scwOctaveLabel->set_vadjustment(*scwNoteEditor->get_vadjustment());

  // connect and setup the CC controls
  m_sb_cc_editor_size->signal_value_changed().
    connect(sigc::mem_fun(this, &DinoGUI::slot_cc_editor_size_changed));
  m_sb_cc_editor_size->set_editable(false);
  
  // connect external signals
  slot<void> update_t_combo = mem_fun(*this, &DinoGUI::update_track_combo);
  slot<void> update_p_combo = mem_fun(*this, &DinoGUI::update_pattern_combo);
  m_song.signal_track_added.connect(hide(update_t_combo));
  m_song.signal_track_removed.connect(hide(update_t_combo));
  
  // setup the pattern properties dialog
  m_dlg_pattern_properties = w<Dialog>("dlg_pattern_properties");
  m_dlgpat_ent_name = w<Entry>("dlgpat_ent_name");
  m_dlgpat_sbn_length = w<SpinButton>("dlgpat_sbn_length");
  m_dlgpat_sbn_steps = w<SpinButton>("dlgpat_sbn_steps");
  m_dlgpat_sbn_cc_steps = w<SpinButton>("dlgpat_sbn_cc_steps");

  // setup the controller properties dialog
  m_dlg_controller_properties = w<Dialog>("dlg_controller_properties");
  m_dlgcont_ent_name = w<Entry>("dlgcont_ent_name");
  HBox* dlgcont_hbx_controller = w<HBox>("dlgcont_hbx_controller");
  dlgcont_hbx_controller->pack_start(m_dlgcont_cmb_controller);
  dlgcont_hbx_controller->show_all();
}


void DinoGUI::init_sequence_editor() {
  // get the widgets
  VBox* boxArrangementEditor = w<VBox>("box_arrangement_editor");
  VBox* box_track_labels  = w<VBox>("box_track_labels");
  Scrollbar* scbHorizontal = w<Scrollbar>("scbh_arrangement_editor");
  Scrollbar* scbVertical = w<Scrollbar>("scbv_arrangement_editor");
  Box* boxSequenceRuler = w<Box>("box_sequence_ruler");
  
  // add the ruler
  EvilScrolledWindow* scwSequenceRuler = 
    manage(new EvilScrolledWindow(true, false));
  boxSequenceRuler->pack_start(*scwSequenceRuler);
  scwSequenceRuler->add(m_sequence_ruler);

  // add the box for the trackwidgets
  EvilScrolledWindow* scwArrangementEditor = manage(new EvilScrolledWindow);
  boxArrangementEditor->pack_start(*scwArrangementEditor);
  m_vbx_track_editor = manage(new VBox(false, 0));
  scwArrangementEditor->add(*m_vbx_track_editor);
  
  // add the box for the track labels
  EvilScrolledWindow* scw_track_labels = manage(new EvilScrolledWindow(false));
  box_track_labels->pack_start(*scw_track_labels);
  m_vbx_track_labels = manage(new VBox(false, 0));
  m_vbx_track_labels->set_border_width(2);
  scw_track_labels->add(*m_vbx_track_labels);
  Viewport* vp = dynamic_cast<Viewport*>(scw_track_labels->get_child());
  if (vp)
    vp->set_shadow_type(SHADOW_NONE);
  
  // synchronise scrolling
  scwSequenceRuler->set_hadjustment(scwArrangementEditor->get_hadjustment());
  scbHorizontal->set_adjustment(*scwArrangementEditor->get_hadjustment());
  scbVertical->set_adjustment(*scwArrangementEditor->get_vadjustment());
  scw_track_labels->set_vadjustment(*scwArrangementEditor->get_vadjustment());
  
  // connect external signals
  slot<void, int> update_track_view = 
    hide(mem_fun(*this, &DinoGUI::update_track_widgets));
  m_song.signal_track_added.connect(update_track_view);
  m_song.signal_track_removed.connect(update_track_view);
  m_song.signal_length_changed.connect(update_track_view);
  m_sequence_ruler.signal_clicked.
    connect(hide(mem_fun(m_seq, &Sequencer::go_to_beat)));
  
  // setup the track properties dialog
  m_dlg_track_properties = w<Dialog>("dlg_track_properties");
  m_dlgtrack_ent_name = w<Entry>("dlgtrack_ent_name");
  m_dlgtrack_sbn_channel = w<SpinButton>("dlgtrack_sbn_channel");
  w<VBox>("dlgtrack_vbx_port")->pack_start(m_dlgtrack_cmb_port);
}


void DinoGUI::init_menus() {
  // connect menu signals
  map<string, void (DinoGUI::*)(void)> menuSlots;
  menuSlots["new1"] = &DinoGUI::slot_file_new;
  menuSlots["open1"] = &DinoGUI::slot_file_open;
  menuSlots["save1"] = &DinoGUI::slot_file_save;
  menuSlots["save_as1"] = &DinoGUI::slot_file_save_as;
  menuSlots["quit1"] = &DinoGUI::slot_file_quit;
  menuSlots["cut1"] = &DinoGUI::slot_edit_cut;
  menuSlots["copy1"] = &DinoGUI::slot_edit_copy;
  menuSlots["paste1"] = &DinoGUI::slot_edit_paste;
  menuSlots["delete1"] = &DinoGUI::slot_edit_delete;
  menuSlots["add_track1"] = &DinoGUI::slot_edit_add_track;
  menuSlots["delete_track1"] = &DinoGUI::slot_edit_delete_track;
  menuSlots["edit_track_properties1"] = 
    &DinoGUI::slot_edit_edit_track_properties;
  menuSlots["add_pattern1"] = &DinoGUI::slot_edit_add_pattern;
  menuSlots["delete_pattern1"] = &DinoGUI::slot_edit_delete_pattern;
  menuSlots["edit_pattern_properties1"] = 
    &DinoGUI::slot_edit_edit_pattern_properties;
  menuSlots["add_controller1"] = &DinoGUI::slot_edit_add_controller;
  menuSlots["delete_controller1"] = &DinoGUI::slot_edit_delete_controller;
  menuSlots["about1"] = &DinoGUI::slot_help_about_dino;
  menuSlots["play1"] = &DinoGUI::slot_transport_play;
  menuSlots["stop1"] = &DinoGUI::slot_transport_stop;
  menuSlots["go_to_start1"] = &DinoGUI::slot_transport_go_to_start;
  map<string, void (DinoGUI::*)(void)>::const_iterator iter;
  for (iter = menuSlots.begin(); iter != menuSlots.end(); ++iter) {
    MenuItem* mi = w<MenuItem>(iter->first);
    assert(mi);
    mi->signal_activate().connect(mem_fun(*this, iter->second));
  }
  
  // and toolbuttons
  map<string, void (DinoGUI::*)(void)> toolSlots;
  toolSlots["tbn_add_pattern"] = &DinoGUI::slot_edit_add_pattern;
  toolSlots["tbn_delete_pattern"] = &DinoGUI::slot_edit_delete_pattern;
  toolSlots["tbn_add_track"] = &DinoGUI::slot_edit_add_track;
  toolSlots["tbn_delete_track"] = &DinoGUI::slot_edit_delete_track;
  toolSlots["tbn_edit_track_properties"] = 
    &DinoGUI::slot_edit_edit_track_properties;
  toolSlots["tbn_play"] = &DinoGUI::slot_transport_play;
  toolSlots["tbn_stop"] = &DinoGUI::slot_transport_stop;
  toolSlots["tbn_go_to_start"] = &DinoGUI::slot_transport_go_to_start;
  for (iter = toolSlots.begin(); iter != toolSlots.end(); ++iter) {
    w<ToolButton>(iter->first)->signal_clicked().
      connect(mem_fun(*this, iter->second));
  }
}


void DinoGUI::init_info_editor() {
  m_ent_title = w<Entry>("ent_title");
  m_ent_author = w<Entry>("ent_author");
  m_text_info = w<TextView>("text_info");
  
  m_song.signal_title_changed.connect(mem_fun(m_ent_title, &Entry::set_text));
  m_song.signal_author_changed.connect(mem_fun(m_ent_author,&Entry::set_text));
  slot<void> set_title = compose(mem_fun(m_song, &Song::set_title),
				 mem_fun(m_ent_title, &Entry::get_text));
  m_ent_title->signal_changed().connect(set_title);
  slot<void> set_author = compose(mem_fun(m_song, &Song::set_author),
				  mem_fun(m_ent_author, &Entry::get_text));
  m_ent_author->signal_changed().connect(set_author);
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
  return (m_lash_client != NULL);
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


void DinoGUI::set_active_track(int active_track) {
  if (active_track != m_active_track) {
    m_active_track = active_track;
    m_conn_pat_added.disconnect();
    m_conn_pat_removed.disconnect();
    if (m_active_track != -1) {
      Track* t = m_song.get_track(m_active_track);
      slot<void> update_slot = mem_fun(*this, &DinoGUI::update_pattern_combo);
      m_conn_pat_added = t->signal_pattern_added.connect(hide(update_slot));
      m_conn_pat_removed= t->signal_pattern_removed.connect(hide(update_slot));
    }
    set_active_pattern(-1);
    signal_active_track_changed(m_active_track);
  }
}


void DinoGUI::set_active_pattern(int active_pattern) {
  if (active_pattern != m_active_pattern) {
    m_active_pattern = active_pattern;
    signal_active_pattern_changed(m_active_track, m_active_pattern);
    m_conn_cont_added.disconnect();
    m_conn_cont_removed.disconnect();
    Pattern* pattern = NULL;
    Track* trk = m_song.get_track(m_active_track);
    if (trk != NULL) {
      pattern = trk->get_pattern(m_active_pattern);
      if (pattern != NULL) {
	slot<void> u_slot = 
	  mem_fun(*this, &DinoGUI::update_controller_combo);
	m_conn_cont_added = 
	  pattern->signal_controller_added.connect(hide(u_slot));
	m_conn_cont_added = 
	  pattern->signal_controller_removed.connect(hide(u_slot));
      }
    }
    m_pe.set_pattern(pattern);
    m_cce.set_pattern(pattern);
    set_active_controller(-1);
    update_controller_combo();
  }
}


void DinoGUI::set_active_controller(int active_controller) {
  if (active_controller != m_active_controller) {
    m_active_controller = active_controller;
    m_cce.set_cc_number(m_active_controller);
    signal_active_controller_changed(m_active_controller);
  }
}


char* DinoGUI::cc_descriptions[] = { "Bank select MSB",
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

				  
