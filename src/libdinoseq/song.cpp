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


  Song::TrackIterator::TrackIterator() {

  }

  
  const Track& Song::TrackIterator::operator*() const { 
    return *(m_iterator->second);
  }
  
  
  Track& Song::TrackIterator::operator*() { 
    return *(m_iterator->second);
  }
  
  
  const Track* Song::TrackIterator::operator->() const { 
    return m_iterator->second;
  }
  
  
  Track* Song::TrackIterator::operator->() { 
    return m_iterator->second;
  }
  
  
  bool Song::TrackIterator::operator==(const TrackIterator& iter) const { 
    return (m_iterator == iter.m_iterator);
  }
  
  
  bool Song::TrackIterator::operator!=(const TrackIterator& iter) const { 
    return (m_iterator != iter.m_iterator);
  }
  

  Song::TrackIterator& Song::TrackIterator::operator++() { 
    ++m_iterator; 
    return *this;
  }


  Song::TrackIterator Song::TrackIterator::operator++(int) {
    TrackIterator result = *this;
    ++(*this);
    return result;
  }
  

  Song::TrackIterator::
  TrackIterator(const std::map<int, Track*>::iterator& iter)
    : m_iterator(iter) {
    
  }


  Song::ConstTrackIterator::ConstTrackIterator() { 

  }
  
  
  Song::ConstTrackIterator::ConstTrackIterator(const TrackIterator& iter) 
    : m_iterator(iter.m_iterator) {

  }
  
  
  const Track& Song::ConstTrackIterator::operator*() const { 
    return *(m_iterator->second); 
  }
  
  
  const Track* Song::ConstTrackIterator::operator->() const { 
    return m_iterator->second; 
  }
  
  
  bool 
  Song::ConstTrackIterator::operator==(const ConstTrackIterator& iter) const { 
    return (m_iterator == iter.m_iterator);
  }
   
  
  bool 
  Song::ConstTrackIterator::operator!=(const ConstTrackIterator& iter) const { 
    return (m_iterator != iter.m_iterator);
  }
  
  
  Song::ConstTrackIterator& Song::ConstTrackIterator::operator++() { 
    ++m_iterator; 
    return *this; 
  }
  
  
  Song::ConstTrackIterator Song::ConstTrackIterator::operator++(int) {
    ConstTrackIterator result = *this;
    ++(*this);
    return result;
  }
  
  
  Song::ConstTrackIterator::
  ConstTrackIterator(const std::map<int, Track*>::const_iterator& iter)
    : m_iterator(iter) {
    
  }
  
  
  Song::TempoIterator::TempoIterator() {
  
  }
      
  
  TempoMap::TempoChange& Song::TempoIterator::operator*() {
    return *m_tempo;
  }
  
      
  TempoMap::TempoChange* Song::TempoIterator::operator->() {
    return m_tempo;
  }
  
  
  bool Song::TempoIterator::operator==(const TempoIterator& iter) const {
    return (m_tempo == iter.m_tempo);
  }
  
  
  bool Song::TempoIterator::operator!=(const TempoIterator& iter) const {
    return (m_tempo != iter.m_tempo);
  }
  
  
  Song::TempoIterator& Song::TempoIterator::operator++() {
    m_tempo = m_tempo->get_next();
    return *this;
  }
  
  
  Song::TempoIterator Song::TempoIterator::operator++(int) {
    TempoIterator iter = *this;
    ++(*this);
    return iter;
  }
  
  
  Song::TempoIterator::TempoIterator(TempoMap::TempoChange* tempo) 
    : m_tempo(tempo) { 
  
  }

  
  Song::Song() 
    : m_tracks(new map<int, Track*>()), 
      m_length(32), 
      m_dirty(false) {
  
    dbg1<<"Initialising song"<<endl;
  
    m_tempo_map.signal_tempochange_added.
      connect(hide(signal_tempo_changed()));
    m_tempo_map.signal_tempochange_changed.
      connect(hide(signal_tempo_changed()));
    m_tempo_map.signal_tempochange_removed.
      connect(hide(signal_tempo_changed()));
  
  }


  Song::~Song() {
    dbg1<<"Destroying song"<<endl;
    map<int, Track*>* tracks = m_tracks;
    map<int, Track*>::iterator iter;
    for (iter = tracks->begin(); iter != tracks->end(); ++iter)
      delete iter->second;
  }


  Song::ConstTrackIterator Song::tracks_begin() const {
    return ConstTrackIterator(m_tracks->begin());
  }
  
  
  Song::TrackIterator Song::tracks_begin() {
    return TrackIterator(m_tracks->begin());
  }
  
  
  Song::ConstTrackIterator Song::tracks_end() const {
    return ConstTrackIterator(m_tracks->end());
  }
  
  
  Song::TrackIterator Song::tracks_end() {
    return TrackIterator(m_tracks->end());
  }
  
  
  Song::ConstTrackIterator Song::tracks_find(int id) const {
    return ConstTrackIterator(m_tracks->find(id));
  }
  
  
  Song::TrackIterator Song::tracks_find(int id) {
    return TrackIterator(m_tracks->find(id));
  }
  

  const size_t Song::get_number_of_tracks() const {
    return m_tracks->size();
  }

  
  void Song::set_title(const string& title) {
    if (title != m_title) {
      dbg1<<"Changing song title to \""<<title<<"\""<<endl;
      m_title = title;
      m_dirty = true;
      m_signal_title_changed(m_title);
    }
  }


  void Song::set_author(const string& author) {
    if (author != m_author) {
      dbg1<<"Changing song author to \""<<author<<"\""<<endl;
      m_author = author;
      m_dirty = true;
      m_signal_author_changed(m_author);
    }
  }


  void Song::set_info(const string& info) {
    if (info != m_info) {
      m_info = info;
      m_dirty = true;
      m_signal_info_changed(m_info);
    }
  }


  void Song::set_length(int length) {
    if (length != m_length) {
      m_length = length;
      map<int, Track*>::iterator iter;
      for (iter = m_tracks->begin(); iter != m_tracks->end(); ++iter)
	iter->second->set_length(length);
      m_signal_length_changed(length);
    }
  }


  Song::TrackIterator Song::add_track(const string& name) {
    map<int, Track*>* new_tracks = new map<int, Track*>(*m_tracks);
    map<int, Track*>::reverse_iterator iter = new_tracks->rbegin();
    int id;
    if (iter == new_tracks->rend())
      id = 1;
    else
      id = iter->first + 1;
    (*new_tracks)[id] = new Track(id, m_length, name);
    m_dirty = true;
    
    map<int, Track*>* old_tracks = m_tracks;
    m_tracks = new_tracks;
    Deleter::queue(old_tracks);
    
    m_signal_track_added(id);
    return TrackIterator(m_tracks->find(id));
  }


  bool Song::remove_track(const Song::TrackIterator& iterator) {
    map<int, Track*>::iterator iter = iterator.m_iterator;
    if (iter == m_tracks->end())
      return false;
    
    // prepare a modified copy of the track map
    map<int, Track*>* new_tracks = new map<int, Track*>(*m_tracks);
    int id = iter->second->get_id();
    Track* trk = iter->second;
    new_tracks->erase(new_tracks->find(id));
    
    // swap the track map and delete the old map and the removed track
    map<int, Track*>* old_tracks = m_tracks;
    m_tracks = new_tracks;
    Deleter::queue(old_tracks);
    Deleter::queue(trk);

    m_dirty = true;
    m_signal_track_removed(id);
    return true;
  }


  Song::TempoIterator Song::add_tempo_change(int beat, double bpm) {
    return TempoIterator(m_tempo_map.add_tempo_change(beat, int(bpm)));
  }


  void Song::remove_tempo_change(TempoIterator& iter) {
    assert(iter != tempo_end());
    m_tempo_map.remove_tempo_change(iter->get_beat());
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
  
  
  int Song::get_length() const {
    return m_length;
  }

  
  Song::TempoIterator Song::tempo_begin() const {
    return TempoIterator(const_cast<TempoMap::TempoChange*>(m_tempo_map.get_changes(0)));
  }
  
  
  Song::TempoIterator Song::tempo_end() const {
    return TempoIterator(0);
  }
  
  
  Song::TempoIterator Song::tempo_find(int beat) const {
    return TempoIterator(const_cast<TempoMap::TempoChange*>(m_tempo_map.get_changes(beat)));
  }
  
  
  double Song::get_current_tempo(double beat) {
    return double(m_tempo_map.get_changes(unsigned(beat))->get_bpm());
  }


  bool Song::is_dirty() const {
    if (m_dirty)
      return true;
    for (map<int, Track*>::const_iterator iter = m_tracks->begin(); 
	 iter != m_tracks->end(); ++iter) {
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
    for ( ; tc != 0; tc = tc->get_next()) {
      Element* tc_elt = tmap_elt->add_child("tempochange");
      char tmp_txt[10];
      sprintf(tmp_txt, "%d", int(tc->get_beat()));
      tc_elt->set_attribute("beat", tmp_txt);
      sprintf(tmp_txt, "%d", tc->get_bpm());
      tc_elt->set_attribute("bpm", tmp_txt);
    }
  
    // write all tracks
    map<int, Track*>::const_iterator iter;
    for (iter = m_tracks->begin(); iter != m_tracks->end(); ++iter) {
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
      if (elt && (text_node = elt->get_child_text()) != 0)
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
    map<int, Track*>* new_tracks = new map<int, Track*>();
    for (iter = nodes.begin(); iter != nodes.end(); ++iter) {
      const Element* track_elt = dynamic_cast<const Element*>(*iter);
      int id;
      sscanf(track_elt->get_attribute("id")->get_value().c_str(), "%d", &id);
      (*new_tracks)[id] = new Track(id, m_length);
      (*new_tracks)[id]->parse_xml_node(track_elt);
      m_signal_track_added(id);
    }
    map<int, Track*>* old_tracks = m_tracks;
    m_tracks = new_tracks;
    Deleter::queue(old_tracks);
  
    return true;
  }


  void Song::clear() {
    m_title = "";
    m_author = "";
    m_info = "";
    m_length = 0;

    map<int, Track*>* old_tracks = m_tracks;
    m_tracks = new map<int, Track*>();
    map<int, Track*>::iterator iter;
    for (iter = old_tracks->begin(); iter != old_tracks->end(); ++iter)
      Deleter::queue(iter->second);
    Deleter::queue(old_tracks);
    
    m_tempo_map = TempoMap();
  }


  unsigned long Song::bt2frame(double beat) {
    return m_tempo_map.get_frame(int32_t(beat), 
				 int32_t(beat - int32_t(beat)) * 10000, 10000);
  }


  pair<int, int> Song::frame2bt(unsigned long frame) {
    int32_t beat = 0, tick = 0;
    return make_pair<int, int>(beat, tick);
  }


  void Song::get_timebase_info(unsigned long frame, unsigned long framerate,
			       double& bpm, double &beat) const {
    m_tempo_map.get_beat(frame, bpm, beat);
  }


  sigc::signal<void, const string&>& Song::signal_title_changed() const {
    return m_signal_title_changed;
  }


  sigc::signal<void, const string&>& Song::signal_author_changed() const {
    return m_signal_author_changed;
  }


  sigc::signal<void, const string&>& Song::signal_info_changed() const {
    return m_signal_info_changed;
  }


  sigc::signal<void, int>& Song::signal_length_changed() const {
    return m_signal_length_changed;
  }


  sigc::signal<void, int>& Song::signal_track_added() const {
    return m_signal_track_added;
  }


  sigc::signal<void, int>& Song::signal_track_removed() const {
    return m_signal_track_removed;
  }


  sigc::signal<void>& Song::signal_tempo_changed() const {
    return m_signal_tempo_changed;
  }


}
