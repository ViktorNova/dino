#include <cstdio>

#include <libxml/tree.h>

#include "song.hpp"


Song::Song() : mDirty(false), mLength(32) {

}
  
void Song::setTitle(const string& title) {
  mTitle = title;
  mDirty = true;
}


void Song::setAuthor(const string& author) {
  mAuthor = author;
  mDirty = true;
}


void Song::setInfo(const string& info) {
  mInfo = info;
  mDirty = true;
}


int Song::addTrack() {
  map<int, Track>::reverse_iterator iter = mTracks.rbegin();
  if (iter == mTracks.rend())
    mTracks[1] = Track(mLength);
  else
    mTracks[iter->first + 1] = Track(mLength);
  mDirty = true;
}


const string& Song::getTitle() const {
  return mTitle;
}


const string& Song::getAuthor() const {
  return mAuthor;
}


const string& Song::getInfo() const {
  return mInfo;
}


const map<int, Track>& Song::getTracks() const {
  return mTracks;
}


map<int, Track>& Song::getTracks() {
  return mTracks;
}


int Song::getLength() const {
  return mLength;
}


bool Song::isDirty() const {
  if (mDirty)
    return true;
  for (map<int, Track>::const_iterator iter = mTracks.begin(); 
       iter != mTracks.end(); ++iter) {
    if (iter->second.isDirty())
      return true;
  }
  return false;
}


const bool Song::writeFile(const string& filename) const {
  xmlDocPtr doc = xmlNewDoc(xmlCharStrdup("1.0"));
  xmlNodePtr dino = xmlNewDocNode(doc, NULL, xmlCharStrdup("dinosong"), NULL);
  xmlDocSetRootElement(doc, dino);
  xmlNewChild(dino, NULL, xmlCharStrdup("title"), 
	      xmlCharStrdup(mTitle.c_str()));
  xmlNewChild(dino, NULL, xmlCharStrdup("author"), 
	      xmlCharStrdup(mAuthor.c_str()));
  xmlNewChild(dino, NULL, xmlCharStrdup("info"), 
	      xmlCharStrdup(mInfo.c_str()));
  
  char idStr[20];
  for (map<int, Track>::const_iterator iter = mTracks.begin(); 
       iter != mTracks.end(); ++iter) {
    xmlNodePtr node = iter->second.getXMLNode(doc);
    sprintf(idStr, "%d", iter->first);
    xmlNewProp(node, xmlCharStrdup("id"), xmlCharStrdup(idStr));
    xmlAddChild(dino, node);
    iter->second.makeClean();
  }
  
  xmlSaveFormatFile(filename.c_str(), doc, 1);
  mDirty = false;
}
