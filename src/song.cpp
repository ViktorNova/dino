#include <cstdio>
#include <iostream>

#include <libxml++/libxml++.h>

#include "song.hpp"


using namespace xmlpp;


Song::Song() : m_dirty(false), m_length(32) {

}
  
void Song::set_title(const string& title) {
  if (title != m_title) {
    m_title = title;
    m_dirty = true;
    signal_title_changed(m_title);
  }
}


void Song::set_author(const string& author) {
  if (author != m_author) {
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


int Song::add_track() {
  map<int, Track>::reverse_iterator iter = m_tracks.rbegin();
  int id;
  if (iter == m_tracks.rend())
    id = 1;
  else
    id = iter->first + 1;
  m_tracks[id] = Track(m_length);
  m_dirty = true;
  signal_track_added(id);
  return id;
}


bool Song::remove_track(int id) {
  map<int, Track>::iterator iter = m_tracks.find(id);
  if (iter == m_tracks.end())
    return false;
  m_tracks.erase(iter);
  m_dirty = true;
  signal_track_removed(id);
  return true;
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


const map<int, Track>& Song::get_tracks() const {
  return m_tracks;
}


map<int, Track>& Song::get_tracks() {
  return m_tracks;
}


int Song::get_length() const {
  return m_length;
}


Mutex& Song::get_big_lock() const {
  return m_big_lock;
}


bool Song::is_dirty() const {
  if (m_dirty)
    return true;
  for (map<int, Track>::const_iterator iter = m_tracks.begin(); 
       iter != m_tracks.end(); ++iter) {
    if (iter->second.is_dirty())
      return true;
  }
  return false;
}


bool Song::write_file(const string& filename) const {
  Document doc;
  Element* dino_elt = doc.create_root_node("dinosong");
  dino_elt->add_child("title")->set_child_text(m_title);
  dino_elt->add_child("author")->set_child_text(m_author);
  dino_elt->add_child("info")->set_child_text(m_info);
  char length_txt[10];
  sprintf(length_txt, "%d", m_length);
  dino_elt->add_child("length")->set_child_text(length_txt);
  map<int, Track>::const_iterator iter;
  for (iter = m_tracks.begin(); iter != m_tracks.end(); ++iter) {
    Element* track_elt = dino_elt->add_child("track");
    char id_txt[10];
    sprintf(id_txt, "%d", iter->first);
    track_elt->set_attribute("id", id_txt);
    if (!iter->second.fill_xml_node(track_elt))
      return false;
  }
  
  doc.write_to_file_formatted(filename);
  m_dirty = false;
}


bool Song::load_file(const string& filename) {
  clear();
  DomParser parser(filename);
  const Document* doc = parser.get_document();
  const Element* dino_elt = doc->get_root_node();
  const TextNode* text_node;
  Node::NodeList::const_iterator iter;
  
  // parse info
  Node::NodeList nodes = dino_elt->get_children();
  for (iter = nodes.begin(); iter != nodes.end(); ++iter) {
    const Element* elt = dynamic_cast<Element*>(*iter);
    if (!elt)
      continue;
    else if (elt->get_name() == "title") {
      if ((text_node = elt->get_child_text()) != NULL) {
	m_title = text_node->get_content();
	signal_title_changed(m_title);
      }
    }
    else if (elt->get_name() == "author") {
      if ((text_node = elt->get_child_text()) != NULL) {
	m_author = text_node->get_content();
	signal_author_changed(m_author);
      }
    }
    else if (elt->get_name() == "info") {
      if ((text_node = elt->get_child_text()) != NULL) {
	m_info = text_node->get_content();
	signal_info_changed(m_info);
      }
    }
    else if (elt->get_name() == "length") {
      sscanf(elt->get_child_text()->get_content().c_str(), "%d", &m_length);
      signal_length_changed(m_length);
    }
  }
  
  // parse all tracks
  nodes = dino_elt->get_children("track");
  for (iter = nodes.begin(); iter != nodes.end(); ++iter) {
    const Element* track_elt = dynamic_cast<const Element*>(*iter);
    int id;
    sscanf(track_elt->get_attribute("id")->get_value().c_str(), "%d", &id);
    m_tracks[id] = Track(m_length);
    m_tracks[id].parse_xml_node(track_elt);
    signal_track_added(id);
  }
}


void Song::clear() {
  set_title("");
  set_author("");
  set_info("");
  signal_length_changed(m_length = 32);
  std::map<int, Track>::const_iterator iter;
  for (iter = m_tracks.begin(); iter != m_tracks.end(); )
    remove_track((iter++)->first);
}
