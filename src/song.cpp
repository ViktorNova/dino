#include <cstdio>

#include <libxml/tree.h>

#include "song.hpp"


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
  xmlDocPtr doc = xmlNewDoc(xmlCharStrdup("1.0"));
  xmlNodePtr dino = xmlNewDocNode(doc, NULL, xmlCharStrdup("dinosong"), NULL);
  xmlDocSetRootElement(doc, dino);
  xmlNewChild(dino, NULL, xmlCharStrdup("title"), 
	      xmlCharStrdup(m_title.c_str()));
  xmlNewChild(dino, NULL, xmlCharStrdup("author"), 
	      xmlCharStrdup(m_author.c_str()));
  xmlNewChild(dino, NULL, xmlCharStrdup("info"), 
	      xmlCharStrdup(m_info.c_str()));
  
  char idStr[20];
  for (map<int, Track>::const_iterator iter = m_tracks.begin(); 
       iter != m_tracks.end(); ++iter) {
    xmlNodePtr node = iter->second.get_xml_node(doc);
    sprintf(idStr, "%d", iter->first);
    xmlNewProp(node, xmlCharStrdup("id"), xmlCharStrdup(idStr));
    xmlAddChild(dino, node);
    iter->second.make_clean();
  }
  
  xmlSaveFormatFile(filename.c_str(), doc, 1);
  m_dirty = false;
}


bool Song::load_file(const string& filename) {
  
}
