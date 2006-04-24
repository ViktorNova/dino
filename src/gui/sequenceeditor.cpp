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


using namespace Gnome::Glade;
using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace Dino;


SequenceEditor::SequenceEditor(BaseObjectType* cobject, 
			       const Glib::RefPtr<Gnome::Glade::Xml>& xml)
  : Gtk::VBox(cobject),
    m_sequence_ruler(32, 1, 4, 20, 20),
    m_dlg_track(0),
    m_active_track(-1),
    m_song(0),
    m_seq(0) {
  
  // get the widgets
  VBox* boxArrangementEditor = w<VBox>(xml, "box_arrangement_editor");
  VBox* box_track_labels  = w<VBox>(xml, "box_track_labels");
  Scrollbar* scbHorizontal = w<Scrollbar>(xml, "scbh_arrangement_editor");
  Scrollbar* scbVertical = w<Scrollbar>(xml, "scbv_arrangement_editor");
  Box* boxSequenceRuler = w<Box>(xml, "box_sequence_ruler");
  m_spb_song_length = w<SpinButton>(xml, "sbn_song_length");
  m_dlg_track = xml->get_widget_derived("dlg_track_properties", m_dlg_track);
  assert(m_dlg_track);

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
  
  // setup the track properties dialog
  m_dlg_track = xml->get_widget_derived("dlg_track_properties", m_dlg_track);

  // and toolbuttons
  typedef std::map<string, void (SequenceEditor::*)(void)> slotmap;
  slotmap toolSlots;
  toolSlots["tbn_add_track"] = &SequenceEditor::add_track;
  toolSlots["tbn_delete_track"] = &SequenceEditor::delete_track;
  toolSlots["tbn_edit_track_properties"] = 
    &SequenceEditor::edit_track_properties;
  toolSlots["tbn_play"] = &SequenceEditor::play;
  toolSlots["tbn_stop"] = &SequenceEditor::stop;
  toolSlots["tbn_go_to_start"] = &SequenceEditor::go_to_start;
  slotmap::const_iterator iter;
  for (iter = toolSlots.begin(); iter != toolSlots.end(); ++iter) {
    m_toolbuttons[iter->first] = w<ToolButton>(xml, iter->first);
    m_toolbuttons[iter->first]->signal_clicked().
      connect(mem_fun(*this, iter->second));
  }

}


void SequenceEditor::set_song(Song* song) {
  m_song = song;
  slot<void, int> update_track_view = 
    sigc::hide(mem_fun(*this, &SequenceEditor::reset_gui));
  m_song->signal_track_added.connect(update_track_view);
  m_song->signal_track_removed.connect(update_track_view);
  m_song->signal_length_changed.connect(update_track_view);
  m_song->signal_length_changed.connect(mem_fun(*m_spb_song_length,
						&SpinButton::set_value));
  m_spb_song_length->signal_value_changed().
    connect(compose(mem_fun(*m_song, &Song::set_length),
		    mem_fun(*m_spb_song_length, &SpinButton::get_value_as_int)));
  m_song->signal_length_changed.
    connect(mem_fun(m_sequence_ruler, &::Ruler::set_length));
}


void SequenceEditor::set_sequencer(Dino::Sequencer* seq) {
  m_seq = seq;
  m_seq->signal_instruments_changed.
    connect(bind(mem_fun(*m_dlg_track, &TrackDialog::update_ports), m_seq));
  m_dlg_track->update_ports(m_seq);
  m_sequence_ruler.signal_clicked.
    connect(sigc::hide(mem_fun(*m_seq, &Sequencer::go_to_beat)));
}


void SequenceEditor::reset_gui() {
  
  m_vbx_track_editor->children().clear();
  m_vbx_track_labels->children().clear();
  
  TempoWidget* tmpw = manage(new TempoWidget(m_song));
  m_vbx_track_editor->pack_start(*tmpw, PACK_SHRINK);
  TempoLabel* tmpl = manage(new TempoLabel(m_song));
  m_vbx_track_labels->pack_start(*tmpl, PACK_SHRINK);
  
  // find the new selected track
  int new_active = -1;
  Song::TrackIterator iter;
  for (iter = m_song->tracks_begin(); iter != m_song->tracks_end(); ++iter) {
    new_active = iter->get_id();
    if (new_active >= m_active_track)
      break;
  }
  m_active_track = new_active;
  
  for (iter = m_song->tracks_begin(); iter != m_song->tracks_end(); ++iter) {
    TrackWidget* tw = manage(new TrackWidget(m_song));
    tw->set_track(&*iter);
    tw->signal_clicked.
      connect(sigc::hide(bind(mem_fun(*this, &SequenceEditor::set_active_track),
			      iter->get_id())));
    m_seq->signal_beat_changed.
      connect(mem_fun(*tw, &TrackWidget::set_current_beat));
    m_vbx_track_editor->pack_start(*tw, PACK_SHRINK);
    TrackLabel* tl = manage(new TrackLabel(m_song));
    tl->set_track(iter->get_id(), &(*iter));
    tl->signal_clicked.
      connect(sigc::hide(bind(mem_fun(*this, &SequenceEditor::set_active_track),
			iter->get_id())));
    m_vbx_track_labels->pack_start(*tl, PACK_SHRINK);
    tl->set_active_track(m_active_track);
  }
  m_vbx_track_editor->show_all();
  m_vbx_track_labels->show_all();
  
  m_toolbuttons["tbn_delete_track"]->set_sensitive(m_active_track != -1);
  m_toolbuttons["tbn_edit_track_properties"]->set_sensitive(m_active_track!=-1);
}


void SequenceEditor::add_track() {
  m_dlg_track->set_name("Untitled");
  m_dlg_track->set_channel(1);
  m_dlg_track->refocus();
  m_dlg_track->show_all();
  if (m_dlg_track->run() == RESPONSE_OK) {
    Song::TrackIterator iter = m_song->add_track(m_dlg_track->get_name());
    iter->set_channel(m_dlg_track->get_channel() - 1);
    m_seq->set_instrument(iter->get_id(), m_dlg_track->get_port());
    set_active_track(iter->get_id());
  }
  m_dlg_track->hide();
}


void SequenceEditor::delete_track() {
  if (m_active_track >= 0) {
    m_song->remove_track(m_song->tracks_find(m_active_track));
  }
}


void SequenceEditor::edit_track_properties() {
  if (m_active_track >= 0) {
    Track& t = *(m_song->tracks_find(m_active_track));
    m_dlg_track->set_name(t.get_name());
    m_dlg_track->set_channel(t.get_channel() + 1);
    m_dlg_track->refocus();
    m_dlg_track->show_all();
    if (m_dlg_track->run() == RESPONSE_OK) {
      t.set_name(m_dlg_track->get_name());
      t.set_channel(m_dlg_track->get_channel() - 1);
      m_seq->set_instrument(m_active_track, m_dlg_track->get_port());
    }
  }
  m_dlg_track->hide();
}


void SequenceEditor::play() {
  m_seq->play();
}


void SequenceEditor::stop() {
  m_seq->stop();
}


void SequenceEditor::go_to_start() {
  m_seq->go_to_beat(0);
}


void SequenceEditor::set_active_track(int track) {
  if (track == m_active_track)
    return;
  m_active_track = track;
  reset_gui();
}
