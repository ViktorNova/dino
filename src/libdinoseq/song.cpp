#include <cstdio>
#include <iostream>
#include <inttypes.h>
#include <cassert>

#include <libxml++/libxml++.h>

#include "debug.hpp"
#include "deleter.hpp"
#include "song.hpp"
#include "track.hpp"


using namespace xmlpp;


namespace Dino {


  Song::Song() : m_length(32), m_dirty(false) {
  
    dbg1<<"Initialising song"<<endl;
  
    m_tempo_map.signal_tempochange_added.connect(hide(signal_tempo_changed));
    m_tempo_map.signal_tempochange_changed.connect(hide(signal_tempo_changed));
    m_tempo_map.signal_tempochange_removed.connect(hide(signal_tempo_changed));
  
  }


  Song::~Song() {
    dbg1<<"Destroying song"<<endl;
    map<int, Track*>::iterator iter;
    for (iter = m_tracks.begin(); iter != m_tracks.end(); ++iter)
      delete iter->second;
  }


  void Song::set_title(const string& title) {
    if (title != m_title) {
      dbg1<<"Changing song title to \""<<title<<"\""<<endl;
      m_title = title;
      m_dirty = true;
      signal_title_changed(m_title);
    }
  }


  void Song::set_author(const string& author) {
    if (author != m_author) {
      dbg1<<"Changing song author to \""<<author<<"\""<<endl;
      m_author = author;
      m_dirty = true;
      signal_author_changed(m_author);
    }
  }


  void Song::set_info(const string& info) {
    if (info != m_info) {
      m_info = info;
      m_dirty = true;
      signal_info_changed(m_info);
    }
  }


  void Song::set_length(int length) {
    // XXX implement this - must be threadsafe since the sequencer uses it
    if (length != m_length) {
      map<int, Track*>::iterator iter;
      for (iter = m_tracks.begin(); iter != m_tracks.end(); ++iter)
	iter->second->set_length(length);
      m_length = length;
      signal_length_changed(length);
    }
  }


  int Song::add_track(const string& name) {
    map<int, Track*>::reverse_iterator iter = m_tracks.rbegin();
    int id;
    if (iter == m_tracks.rend())
      id = 1;
    else
      id = iter->first + 1;
    m_tracks[id] = new Track(m_length, name);
    m_dirty = true;
    signal_track_added(id);
    return id;
  }


  bool Song::remove_track(int id) {
    map<int, Track*>::iterator iter = m_tracks.find(id);
    if (iter == m_tracks.end())
      return false;
    iter->second->queue_deletion();
    m_tracks.erase(iter);
    m_dirty = true;
    signal_track_removed(id);
    return true;
  }


  void Song::add_tempo_change(int beat, double bpm) {
    m_tempo_map.add_tempo_change(beat, int(bpm));
  }


  void Song::remove_tempo_change(int beat) {
    m_tempo_map.remove_tempo_change(beat);
  }


  const string& Song::get_title() const {
    return m_title;
  }


  const string& Song::get_author() const {
    return m_author;
  }


  const string& Song::get_info() const {
    return m_info;
  }


  const map<int, Track*>& Song::get_tracks() const {
    return m_tracks;
  }


  map<int, Track*>& Song::get_tracks() {
    return m_tracks;
  }


  const Track* Song::get_track(int id) const {
    return get_track(id);
  }
  
  
  Track* Song::get_track(int id) {
    map<int, Track*>::iterator iter = m_tracks.find(id);
    assert(iter != m_tracks.end());
    return iter->second;
  }


  int Song::get_length() const {
    return m_length;
  }


  const TempoMap::TempoChange* Song::get_tempo_changes() const {
    return m_tempo_map.get_changes(0);
  }


  double Song::get_current_tempo(int beat, int tick) {
    return double(m_tempo_map.get_changes(beat)->bpm);
  }


  bool Song::is_dirty() const {
    if (m_dirty)
      return true;
    for (map<int, Track*>::const_iterator iter = m_tracks.begin(); 
	 iter != m_tracks.end(); ++iter) {
      if (iter->second->is_dirty())
	return true;
    }
    return false;
  }


  bool Song::write_file(const string& filename) const {
    Document doc;
    Element* dino_elt = doc.create_root_node("dinosong");
  
    // write metadata 
    dino_elt->set_attribute("title", m_title);
    dino_elt->set_attribute("author", m_author);
    dino_elt->add_child("info")->set_child_text(m_info);
  
    // write length
    char length_txt[10];
    sprintf(length_txt, "%d", m_length);
    dino_elt->set_attribute("length", length_txt);
  
    // write the tempomap
    const TempoMap::TempoChange* tc = m_tempo_map.get_changes(0);
    Element* tmap_elt = dino_elt->add_child("tempomap");
    for ( ; tc != NULL; tc = tc->next) {
      Element* tc_elt = tmap_elt->add_child("tempochange");
      char tmp_txt[10];
      sprintf(tmp_txt, "%d", int(tc->beat));
      tc_elt->set_attribute("beat", tmp_txt);
      sprintf(tmp_txt, "%d", tc->bpm);
      tc_elt->set_attribute("bpm", tmp_txt);
    }
  
    // write all tracks
    map<int, Track*>::const_iterator iter;
    for (iter = m_tracks.begin(); iter != m_tracks.end(); ++iter) {
      Element* track_elt = dino_elt->add_child("track");
      char id_txt[10];
      sprintf(id_txt, "%d", iter->first);
      track_elt->set_attribute("id", id_txt);
      if (!iter->second->fill_xml_node(track_elt))
	return false;
    }
  
    doc.write_to_file_formatted(filename);
    m_dirty = false;
    return true;
  }


  bool Song::load_file(const string& filename) {
    clear();
    DomParser parser(filename);
    const Document* doc = parser.get_document();
    const Element* dino_elt = doc->get_root_node();
    const TextNode* text_node;
    Node::NodeList::const_iterator iter;
  
    // get attributes
    set_title(dino_elt->get_attribute("title")->get_value());
    set_author(dino_elt->get_attribute("author")->get_value());
    sscanf(dino_elt->get_attribute("length")->get_value().c_str(), 
	   "%d", &m_length);
  
    // get info
    Node::NodeList nodes = dino_elt->get_children("info");
    if (nodes.begin() != nodes.end()) {
      const Element* elt = dynamic_cast<Element*>(*nodes.begin());
      if (elt && (text_node = elt->get_child_text()) != NULL)
	set_info(text_node->get_content());
    }
  
    // parse tempomap
    nodes = dino_elt->get_children("tempomap");
    if (nodes.begin() != nodes.end()) {
      const Element* elt = dynamic_cast<Element*>(*nodes.begin());
      Node::NodeList nodes2 = elt->get_children("tempochange");
      for (iter = nodes2.begin(); iter != nodes2.end(); ++iter) {
	const Element* tc_elt = dynamic_cast<Element*>(*iter);
	int beat, bpm;
	sscanf(tc_elt->get_attribute("beat")->get_value().c_str(), "%d", &beat);
	sscanf(tc_elt->get_attribute("bpm")->get_value().c_str(), "%d", &bpm);
	m_tempo_map.add_tempo_change(beat, bpm);
      }
    }
  
    // parse all tracks
    nodes = dino_elt->get_children("track");
    for (iter = nodes.begin(); iter != nodes.end(); ++iter) {
      const Element* track_elt = dynamic_cast<const Element*>(*iter);
      int id;
      sscanf(track_elt->get_attribute("id")->get_value().c_str(), "%d", &id);
      m_tracks[id] = new Track(m_length);
      m_tracks[id]->parse_xml_node(track_elt);
      signal_track_added(id);
    }
  
    return true;
  }


  void Song::clear() {
    m_title = "";
    m_author = "";
    m_info = "";
    m_length = 0;
    m_tracks.clear();
    m_tempo_map = TempoMap();
  }


  unsigned long Song::bt2frame(double beat) {
    return m_tempo_map.get_frame(int32_t(beat), 
				 int32_t(beat - int32_t(beat)) * 10000, 10000);
  }


  pair<int, int> Song::frame2bt(unsigned long frame) {
    double bpm;
    int32_t beat, tick;
    m_tempo_map.get_bbt(frame, 48000, bpm, beat, tick);
    return make_pair<int, int>(beat, tick);
  }


  void Song::get_timebase_info(unsigned long frame, unsigned long framerate,
			       double& bpm, int32_t& beat, int32_t& tick) const {
    m_tempo_map.get_bbt(frame, framerate, bpm, beat, tick);
  }


}
