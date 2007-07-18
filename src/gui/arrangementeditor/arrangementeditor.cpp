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

#include <map>

#include "evilscrolledwindow.hpp"
#include "arrangementeditor.hpp"
#include "commandproxy.hpp"
#include "sequencer.hpp"
#include "song.hpp"
#include "track.hpp"
#include "tempowidget.hpp"
#include "tempolabel.hpp"
#include "trackwidget.hpp"
#include "tracklabel.hpp"
#include "plugininterface.hpp"


using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace std;
using namespace Dino;


namespace {
  ArrangementEditor* m_se;
  PluginInterface* m_plif;
  
  void print_action_name(::Action& action) {
    std::cerr<<"Action added: "<<action.get_name()<<std::endl;
  }
  
}


extern "C" {
  
  string dino_get_name() { 
    return "Arrangement editor"; 
  }
  
  void dino_load_plugin(PluginInterface& plif) {
    m_plif = &plif;
    m_se = manage(new ArrangementEditor(plif));
    plif.add_page("Arrangement", *m_se);
    plif.signal_action_added().connect(&print_action_name);
  }
  
  void dino_unload_plugin() {
    m_plif->remove_page(*m_se); 
  }
}


ArrangementEditor::ArrangementEditor(PluginInterface& plif)
  : m_sequence_ruler(32, 1, 4, 20, 20),
    m_vbx_track_editor(false, 0),
    m_vbx_track_labels(false, 0),
    m_active_track(-1),
    m_song(plif.get_song()),
    m_seq(plif.get_sequencer()),
    m_proxy(plif.get_command_proxy()),
    m_plif(plif) {
  
  VBox* v = manage(new VBox);
  
  // initialise the toolbar
  m_tooltips.enable();
  Toolbar* tbar = manage(new Toolbar);
  tbar->set_toolbar_style(TOOLBAR_ICONS);
  add_toolbutton(tbar, m_tbn_add_track, Stock::ADD, "Create new track",
                 &ArrangementEditor::add_track);
  add_toolbutton(tbar, m_tbn_delete_track, Stock::DELETE, 
                 "Delete the selected track", &ArrangementEditor::delete_track);
  add_toolbutton(tbar, m_tbn_edit_track_properties, Stock::PROPERTIES, 
                 "Edit track properties", 
                 &ArrangementEditor::edit_track_properties);

  tbar->append(*manage(new SeparatorToolItem));
  add_toolbutton(tbar, m_tbn_play, Stock::MEDIA_PLAY, "Play",
                 &ArrangementEditor::play);
  add_toolbutton(tbar, m_tbn_stop, Stock::MEDIA_PAUSE, "Stop",
                 &ArrangementEditor::stop);
  add_toolbutton(tbar, m_tbn_go_to_start, Stock::MEDIA_PREVIOUS, "Go to start",
                 &ArrangementEditor::go_to_start);
  
  tbar->append(*manage(new SeparatorToolItem));
  HBox* thbx = manage(new HBox(false, 5));
  thbx->pack_start(*manage(new Label("Song length:")));
  m_spb_song_length.set_range(1, 10000);
  m_spb_song_length.set_increments(1, 16);
  m_tooltips.set_tip(m_spb_song_length, "Set song length");
  thbx->pack_start(m_spb_song_length);
  ToolItem* ti = manage(new ToolItem);
  ti->add(*thbx);
  tbar->append(*ti);
  v->pack_start(*tbar, false, true);
  
  // add the ruler
  Table* table = manage(new Table(3, 3));
  table->set_row_spacings(3);
  table->set_col_spacings(3);
  table->set_border_width(3);
  v->pack_start(*table);
  EvilScrolledWindow* scw_ruler = manage(new EvilScrolledWindow(true, false));
  scw_ruler->add(m_sequence_ruler);
  table->attach(*scw_ruler, 1, 2, 0, 1, FILL, FILL);
  
  // add the scrollbars
  HScrollbar* hscroll = manage(new HScrollbar);
  VScrollbar* vscroll = manage(new VScrollbar);
  table->attach(*vscroll, 2, 3, 1, 2, FILL, FILL);
  table->attach(*hscroll, 1, 2, 2, 3, FILL, FILL);

  // add the box for the trackwidgets
  EvilScrolledWindow* scw_trackwidgets = manage(new EvilScrolledWindow);
  table->attach(*scw_trackwidgets, 1, 2, 1, 2);
  scw_trackwidgets->add(m_vbx_track_editor);
  
  // add the box for the track labels
  EvilScrolledWindow* scw_track_labels = manage(new EvilScrolledWindow(false));
  table->attach(*scw_track_labels, 0, 1, 1, 2, FILL);
  m_vbx_track_labels.set_border_width(2);
  scw_track_labels->add(m_vbx_track_labels);
  scw_track_labels->set_shadow_type(SHADOW_NONE);

  // synchronise scrolling
  scw_ruler->set_hadjustment(scw_trackwidgets->get_hadjustment());
  hscroll->set_adjustment(*scw_trackwidgets->get_hadjustment());
  vscroll->set_adjustment(*scw_trackwidgets->get_vadjustment());
  scw_track_labels->set_vadjustment(*scw_trackwidgets->get_vadjustment());
  
  pack_start(*v);
  
  // add a new tempo widget and a tempo label
  TempoWidget* tmpw = manage(new TempoWidget(m_proxy, &m_song));
  m_vbx_track_editor.pack_start(*tmpw, PACK_SHRINK);
  slot<void> update_menu = bind(mem_fun(*tmpw, &TempoWidget::update_menu), 
                                ref(m_plif));
  m_plif.signal_action_added().connect(sigc::hide(update_menu));
  m_plif.signal_action_removed().connect(sigc::hide(update_menu));
  update_menu();
  TempoLabel* tmpl = manage(new TempoLabel(&m_song));
  m_vbx_track_labels.pack_start(*tmpl, PACK_SHRINK);
  
  // set song length
  m_spb_song_length.set_value(m_song.get_length());
  
  // add track widgets
  Song::TrackIterator tit;
  for (tit = m_song.tracks_begin(); tit != m_song.tracks_end(); ++tit)
    track_added(tit->get_id());
  if (m_song.tracks_begin() != m_song.tracks_end())
    set_active_track(m_song.tracks_begin()->get_id());
  else
    set_active_track(-1);
  
  // connect to the song
  m_song.signal_track_added().
    connect(mem_fun(*this, &ArrangementEditor::track_added));
  m_song.signal_track_removed().
    connect(mem_fun(*this, &ArrangementEditor::track_removed));
  m_song.signal_length_changed().
    connect(mem_fun(m_spb_song_length, &SpinButton::set_value));
  m_song.signal_length_changed().
    connect(mem_fun(m_sequence_ruler, &::Ruler::set_length));
  m_song.signal_loop_start_changed().
    connect(mem_fun(m_sequence_ruler, &::Ruler::set_loop_start));
  m_song.signal_loop_end_changed().
    connect(mem_fun(m_sequence_ruler, &::Ruler::set_loop_end));
  m_spb_song_length.signal_value_changed().
    connect(compose(hide_return(mem_fun(m_proxy, 
					&CommandProxy::set_song_length)),
  		    mem_fun(m_spb_song_length, &SpinButton::get_value_as_int)));
  
  // connect to sequencer
  m_seq.signal_instruments_changed().
    connect(bind(mem_fun(m_dlg_track, &TrackDialog::update_ports), &m_seq));
  m_seq.signal_record_to_track().
    connect(mem_fun(*this, &ArrangementEditor::set_recording_track));
  m_dlg_track.update_ports(&m_seq);
  m_sequence_ruler.signal_clicked.
    connect(mem_fun(*this, &ArrangementEditor::ruler_clicked));
  
}


void ArrangementEditor::add_track() {
  m_dlg_track.reset();
  m_dlg_track.show_all();
  if (m_dlg_track.run() == RESPONSE_OK) {
    Song::TrackIterator iter;
    m_proxy.add_track(m_dlg_track.get_name(), &iter);
    m_dlg_track.apply_to_track(*iter, m_seq);
    set_active_track(iter->get_id());
  }
  m_dlg_track.hide();
}


void ArrangementEditor::delete_track() {
  if (m_active_track >= 0)
    m_proxy.remove_track(m_active_track);
}


void ArrangementEditor::edit_track_properties() {
  if (m_active_track >= 0) {
    Track& t = *(m_song.tracks_find(m_active_track));
    m_dlg_track.set_track(t, m_seq);
    m_dlg_track.show_all();
    if (m_dlg_track.run() == RESPONSE_OK) {
      m_dlg_track.apply_to_track(t, m_seq);
    }
  }
  m_dlg_track.hide();
}


void ArrangementEditor::play() {
  m_seq.play();
}


void ArrangementEditor::stop() {
  m_seq.stop();
}


void ArrangementEditor::go_to_start() {
  m_seq.go_to_beat(0);
}


void ArrangementEditor::set_active_track(int track) {
  if (track == m_active_track)
    return;
  m_active_track = track;
  std::map<int, SingleTrackGUI>::iterator iter;
  for (iter = m_track_map.begin(); iter != m_track_map.end(); ++iter)
    iter->second.label->set_active_track(track);
  m_tbn_delete_track->set_sensitive(m_active_track != -1);
  m_tbn_edit_track_properties->set_sensitive(m_active_track!=-1);
}


void ArrangementEditor::ruler_clicked(double beat, int button) {
  if (button == 2)
    m_seq.go_to_beat(beat);
  else if (button == 1)
    m_proxy.set_loop_start(int(beat));
  else if (button == 3)
    m_proxy.set_loop_end(int(ceil(beat)));
}


void ArrangementEditor::add_toolbutton(Gtk::Toolbar* tbar, 
                                    Gtk::ToolButton*& tbutton, 
                                    Gtk::BuiltinStockID stock, 
                                    const std::string& tip,
                                    void (ArrangementEditor::*button_slot)()) {
  Image* img = manage(new Image(stock, ICON_SIZE_SMALL_TOOLBAR));
  tbutton = manage(new ToolButton(*img));
  tbutton->set_tooltip(m_tooltips, tip);
  tbar->append(*tbutton, mem_fun(*this, button_slot));
}


void ArrangementEditor::set_recording_track(Song::TrackIterator iter) {
  int id = (iter == m_song.tracks_end() ? -1 : iter->get_id());
  std::map<int, SingleTrackGUI>::iterator i;
  for (i = m_track_map.begin(); i != m_track_map.end(); ++i)
    i->second.label->set_recording(i->first == id);
}


void ArrangementEditor::track_added(int track) {
  
  Song::TrackIterator iter = m_song.tracks_find(track);
  
  assert(m_song.tracks_find(track) != m_song.tracks_end());
  
  set_active_track(track);
  
  bool recording = false;
  Song::TrackIterator rec_track = m_seq.get_recording_track();
  if (rec_track == iter)
    recording = true;
  
  // add track widget
  TrackWidget* tw = manage(new TrackWidget(m_proxy));
  tw->signal_status.
    connect(hide_return(bind(mem_fun(m_plif, &PluginInterface::set_status),
			     3000)));
  tw->set_track(&*iter);
  slot<void> update_menu = bind(mem_fun(*tw, &TrackWidget::update_menu), 
				ref(m_plif));
  m_plif.signal_action_added().connect(sigc::hide(update_menu));
  m_plif.signal_action_removed().connect(sigc::hide(update_menu));
  update_menu();
  tw->signal_clicked.
    connect(sigc::hide(bind(mem_fun(*this, 
				    &ArrangementEditor::set_active_track),
			    iter->get_id())));
  m_seq.signal_beat_changed().
    connect(mem_fun(*tw, &TrackWidget::set_current_beat));
  m_vbx_track_editor.pack_start(*tw, PACK_SHRINK);
  m_vbx_track_editor.show_all();
  
  // add track label
  TrackLabel* tl = manage(new TrackLabel(&m_song));
  tl->set_track(iter->get_id(), &(*iter));
  tl->signal_clicked.
    connect(sigc::hide(bind(mem_fun(*this, &ArrangementEditor::set_active_track),
			    iter->get_id())));
  m_vbx_track_labels.pack_start(*tl, PACK_SHRINK);
  m_vbx_track_labels.show_all();
  tl->set_active_track(m_active_track);
  tl->set_recording(recording);
  
  m_track_map[iter->get_id()] = SingleTrackGUI(tl, tw);
  
}


void ArrangementEditor::track_removed(int track) {
  
  assert(m_track_map[track].label);
  assert(m_track_map[track].widget);
  
  if (m_track_map[track].label)
    m_vbx_track_labels.remove(*m_track_map[track].label);
  if (m_track_map[track].widget)
    m_vbx_track_editor.remove(*m_track_map[track].widget);
  
  if (m_song.tracks_find(m_active_track) == m_song.tracks_end())
    set_active_track(-1);

}

