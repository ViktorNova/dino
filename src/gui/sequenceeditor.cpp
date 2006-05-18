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

#include <map>

#include "evilscrolledwindow.hpp"
#include "sequenceeditor.hpp"
#include "sequencer.hpp"
#include "song.hpp"
#include "track.hpp"
#include "tempowidget.hpp"
#include "tempolabel.hpp"
#include "trackwidget.hpp"
#include "tracklabel.hpp"


using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace Dino;


namespace {
  SequenceEditor* m_se;
  PluginInterface* m_plif;
}


extern "C" {
  
  string dino_get_name() { 
    return "Sequence editor"; 
  }
  
  void dino_load_plugin(PluginInterface& plif) {
    m_plif = &plif;
    m_se = manage(new SequenceEditor(plif.get_song(), plif.get_sequencer()));
    plif.add_page("Arrangement", *m_se);
  }
  
  void dino_unload_plugin() {
    m_plif->remove_page(*m_se); 
  }
}


SequenceEditor::SequenceEditor(Dino::Song& song, Dino::Sequencer& seq)
  : m_sequence_ruler(32, 1, 4, 20, 20),
    m_dlg_track(0),
    m_active_track(-1),
    m_song(song),
    m_seq(seq) {
  
  VBox* v = manage(new VBox);
  
  // initialise the toolbar
  Toolbar* tbar = manage(new Toolbar);
  tbar->set_toolbar_style(TOOLBAR_ICONS);
  m_tbn_add_track = manage(new ToolButton(*manage(new Image(Stock::ADD, ICON_SIZE_SMALL_TOOLBAR))));
  tbar->append(*m_tbn_add_track, mem_fun(*this, &SequenceEditor::add_track));
  m_tbn_delete_track = manage(new ToolButton(*manage(new Image(Stock::DELETE, ICON_SIZE_SMALL_TOOLBAR))));
  tbar->append(*m_tbn_delete_track, mem_fun(*this, &SequenceEditor::delete_track));
  m_tbn_edit_track_properties = manage(new ToolButton(*manage(new Image(Stock::PROPERTIES, ICON_SIZE_SMALL_TOOLBAR))));
  tbar->append(*m_tbn_edit_track_properties, mem_fun(*this, &SequenceEditor::edit_track_properties));
  tbar->append(*manage(new SeparatorToolItem));
  m_tbn_play = manage(new ToolButton(*manage(new Image(Stock::MEDIA_PLAY, ICON_SIZE_SMALL_TOOLBAR))));
  tbar->append(*m_tbn_play, mem_fun(*this, &SequenceEditor::play));
  m_tbn_stop = manage(new ToolButton(*manage(new Image(Stock::MEDIA_PAUSE, ICON_SIZE_SMALL_TOOLBAR))));
  tbar->append(*m_tbn_stop, mem_fun(*this, &SequenceEditor::stop));
  m_tbn_go_to_start = manage(new ToolButton(*manage(new Image(Stock::MEDIA_PREVIOUS, ICON_SIZE_SMALL_TOOLBAR))));
  tbar->append(*m_tbn_go_to_start, mem_fun(*this, &SequenceEditor::go_to_start));
  tbar->append(*manage(new SeparatorToolItem));
  HBox* thbx = manage(new HBox(false, 5));
  thbx->pack_start(*manage(new Label("Song length:")));
  m_spb_song_length = manage(new SpinButton);
  m_spb_song_length->set_range(1, 10000);
  m_spb_song_length->set_increments(1, 16);
  thbx->pack_start(*m_spb_song_length);
  ToolItem* ti = manage(new ToolItem);
  ti->add(*thbx);
  tbar->append(*ti);
  v->pack_start(*tbar, false, true);
  
  // add the ruler
  Table* table = manage(new Table(3, 3));
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
  m_vbx_track_editor = manage(new VBox(false, 0));
  scw_trackwidgets->add(*m_vbx_track_editor);
  
  // add the box for the track labels
  EvilScrolledWindow* scw_track_labels = manage(new EvilScrolledWindow(false));
  table->attach(*scw_track_labels, 0, 1, 1, 2, FILL);
  m_vbx_track_labels = manage(new VBox(false, 0));
  m_vbx_track_labels->set_border_width(2);
  scw_track_labels->add(*m_vbx_track_labels);
  scw_track_labels->set_shadow_type(SHADOW_NONE);
  
  // synchronise scrolling
  scw_ruler->set_hadjustment(scw_trackwidgets->get_hadjustment());
  hscroll->set_adjustment(*scw_trackwidgets->get_hadjustment());
  vscroll->set_adjustment(*scw_trackwidgets->get_vadjustment());
  scw_track_labels->set_vadjustment(*scw_trackwidgets->get_vadjustment());
  
  pack_start(*v);
  
  m_dlg_track = new TrackDialog;
  
  // connect to the song
  slot<void, int> update_track_view = 
    sigc::hide(mem_fun(*this, &SequenceEditor::reset_gui));
  m_song.signal_track_added().connect(update_track_view);
  m_song.signal_track_removed().connect(update_track_view);
  m_song.signal_length_changed().connect(update_track_view);
  m_song.signal_length_changed().connect(mem_fun(*m_spb_song_length,
  						&SpinButton::set_value));
  m_spb_song_length->signal_value_changed().
    connect(compose(mem_fun(m_song, &Song::set_length),
  		    mem_fun(*m_spb_song_length, &SpinButton::get_value_as_int)));
  m_song.signal_length_changed().
    connect(mem_fun(m_sequence_ruler, &::Ruler::set_length));
  m_song.signal_loop_start_changed().
    connect(mem_fun(m_sequence_ruler, &::Ruler::set_loop_start));
  m_song.signal_loop_end_changed().
    connect(mem_fun(m_sequence_ruler, &::Ruler::set_loop_end));
  
  // connect to sequencer
  m_seq.signal_instruments_changed().
    connect(bind(mem_fun(*m_dlg_track, &TrackDialog::update_ports), &m_seq));
  m_dlg_track->update_ports(&m_seq);
  m_sequence_ruler.signal_clicked.
    connect(mem_fun(*this, &SequenceEditor::ruler_clicked));
  
  reset_gui();
}


void SequenceEditor::reset_gui() {
  
  m_spb_song_length->set_value(m_song.get_length());
  
  m_vbx_track_editor->children().clear();
  m_vbx_track_labels->children().clear();
  
  TempoWidget* tmpw = manage(new TempoWidget(&m_song));
  m_vbx_track_editor->pack_start(*tmpw, PACK_SHRINK);
  TempoLabel* tmpl = manage(new TempoLabel(&m_song));
  m_vbx_track_labels->pack_start(*tmpl, PACK_SHRINK);
  
  // find the new selected track
  int new_active = -1;
  Song::TrackIterator iter;
  for (iter = m_song.tracks_begin(); iter != m_song.tracks_end(); ++iter) {
    new_active = iter->get_id();
    if (new_active >= m_active_track)
      break;
  }
  m_active_track = new_active;
  
  for (iter = m_song.tracks_begin(); iter != m_song.tracks_end(); ++iter) {
    TrackWidget* tw = manage(new TrackWidget(&m_song));
    tw->set_track(&*iter);
    tw->signal_clicked.
      connect(sigc::hide(bind(mem_fun(*this, &SequenceEditor::set_active_track),
			      iter->get_id())));
    m_seq.signal_beat_changed().
      connect(mem_fun(*tw, &TrackWidget::set_current_beat));
    m_vbx_track_editor->pack_start(*tw, PACK_SHRINK);
    TrackLabel* tl = manage(new TrackLabel(&m_song));
    tl->set_track(iter->get_id(), &(*iter));
    tl->signal_clicked.
      connect(sigc::hide(bind(mem_fun(*this, &SequenceEditor::set_active_track),
			iter->get_id())));
    m_vbx_track_labels->pack_start(*tl, PACK_SHRINK);
    tl->set_active_track(m_active_track);
  }
  m_vbx_track_editor->show_all();
  m_vbx_track_labels->show_all();
  
  m_tbn_delete_track->set_sensitive(m_active_track != -1);
  m_tbn_edit_track_properties->set_sensitive(m_active_track!=-1);
}


void SequenceEditor::add_track() {
  m_dlg_track->set_name("Untitled");
  m_dlg_track->set_channel(1);
  m_dlg_track->refocus();
  m_dlg_track->show_all();
  if (m_dlg_track->run() == RESPONSE_OK) {
    Song::TrackIterator iter = m_song.add_track(m_dlg_track->get_name());
    iter->set_channel(m_dlg_track->get_channel() - 1);
    m_seq.set_instrument(iter->get_id(), m_dlg_track->get_port());
    set_active_track(iter->get_id());
  }
  m_dlg_track->hide();
}


void SequenceEditor::delete_track() {
  if (m_active_track >= 0) {
    m_song.remove_track(m_song.tracks_find(m_active_track));
  }
}


void SequenceEditor::edit_track_properties() {
  if (m_active_track >= 0) {
    Track& t = *(m_song.tracks_find(m_active_track));
    m_dlg_track->set_name(t.get_name());
    m_dlg_track->set_channel(t.get_channel() + 1);
    m_dlg_track->refocus();
    m_dlg_track->show_all();
    if (m_dlg_track->run() == RESPONSE_OK) {
      t.set_name(m_dlg_track->get_name());
      t.set_channel(m_dlg_track->get_channel() - 1);
      m_seq.set_instrument(m_active_track, m_dlg_track->get_port());
    }
  }
  m_dlg_track->hide();
}


void SequenceEditor::play() {
  m_seq.play();
}


void SequenceEditor::stop() {
  m_seq.stop();
}


void SequenceEditor::go_to_start() {
  m_seq.go_to_beat(0);
}


void SequenceEditor::set_active_track(int track) {
  if (track == m_active_track)
    return;
  m_active_track = track;
  reset_gui();
}


void SequenceEditor::ruler_clicked(double beat, int button) {
  if (button == 2)
    m_seq.go_to_beat(beat);
  else if (button == 1)
    m_song.set_loop_start(floor(beat));
  else if (button == 3)
    m_song.set_loop_end(ceil(beat));
}

