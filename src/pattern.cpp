#include <algorithm>
#include <cassert>
#include <iostream>

#include "pattern.hpp"


Pattern::Pattern() : mDirty(false), nextNote(NULL) {
  
}


Pattern::Pattern(int length, int steps, int ccSteps) : mDirty(false) {
  this->length = length;
  this->steps = steps;
  this->ccSteps = ccSteps;
  minStep = length * steps;
  maxStep = -1;
  minNote = 128;
  maxNote = -1;
}


void Pattern::addNote(int step, int value, int noteLength) {
  assert(step >= 0 && step < length * steps);
  assert(value >= 0 && value < 128);
  assert(noteLength > 0);
  assert(step + noteLength <= length * steps);
  pair<int, int> p = make_pair(step, value);
  Note* previous = NULL;
  Note* next = NULL;
  NoteMap::iterator iter;
  
  for (iter = notes.begin(); iter != notes.end(); ++iter) {
    
    // we're looking at a note that's before the new one
    if (iter->first.first < step || 
	(iter->first.first == step && iter->first.second < value)) {
      previous = iter->second;
      // it could be overlapping, if so, shorten it
      if (iter->first.second == value && 
	  iter->first.first + iter->second->length > step) {
	maxStep = (iter->first.first + iter->second->length > maxStep ? 
		   iter->first.first + iter->second->length : maxStep);
	iter->second->length = step - iter->first.first;
      }
    }
    
    // we're looking at a note with the same pitch and start time - delete it
    else if (iter->first.first == step && iter->first.second == value) {
      delete iter->second;
      maxStep = (iter->first.first + iter->second->length > maxStep ?
		 iter->first.first + iter->second->length : maxStep);
    }
    
    // we're looking at a node after the new one - link to it and exit the loop
    else if (iter->first.first > step ||
	     (iter->first.first == step && iter->first.second > value)) {
      next = iter->second;
      break;
    }
  }
  
  // check if there is a note that begins before the end of the new note,
  // if so shorten the new note
  for (iter = notes.upper_bound(p); iter != notes.end(); ++iter) {
    if (iter->first.second == value) {
      if (iter->first.first > step && step + noteLength > iter->first.first)
	noteLength = iter->first.first - step;
      break;
    }
  }
  
  // create the note, set up links for the sequencer list and put it in the
  // map
  Note* note = new Note(p.first, p.second, noteLength);
  note->previous = previous;
  note->next = next;
  if (next)
    next->previous = note;
  if (previous)
    previous->next = note;
  notes[p] = note;
  
  minNote = value < minNote ? value : minNote;
  maxNote = value > maxNote ? value : maxNote;
  minStep = step < minStep ? step : minStep;
  maxStep = step + noteLength - 1 > maxStep ? step + noteLength - 1 : maxStep;
}


int Pattern::deleteNote(int step, int value) {
  assert(step >= 0 && step < length * steps);
  assert(value >= 0 && value < 128);
  for (NoteMap::iterator iter = notes.begin(); 
       iter != notes.end() && iter->first.first <= step; ++iter) {
    if (iter->first.second == value && iter->first.first <= step &&
	iter->first.first + iter->second->length > step) {
      int result = iter->first.first;
      minNote = iter->first.second < minNote ? iter->first.second : minNote;
      maxNote = iter->first.second > maxNote ? iter->first.second : maxNote;
      minStep = iter->first.first < minStep ? iter->first.first : minStep;
      maxStep = (iter->first.first + iter->second->length - 1 > maxStep ? 
		 iter->first.first + iter->second->length - 1 : maxStep);
      if (iter->second->previous)
	iter->second->previous->next = iter->second->next;
      if (iter->second->next)
	iter->second->next->previous = iter->second->previous;
      delete iter->second;
      notes.erase(iter);
      return result;
    }
  }
  return -1;
}


void Pattern::addCC(int ccNumber, int step, int value) {
  assert(ccNumber >= 0 && ccNumber < 128);
  assert(step >= 0 && step < length * ccSteps);
  assert(value >= 0 && value < 128);
  controlChanges[ccNumber].changes[step] = new CCEvent(ccNumber, value);
}


int Pattern::deleteCC(int ccNumber, int step) {
  assert(ccNumber >= 0 && ccNumber < 128);
  assert(step >= 0 && step < length * ccSteps);
  CCData& data(controlChanges[ccNumber]);
  map<int, CCEvent*>::iterator iter = data.changes.find(step);
  if (iter != data.changes.end()) {
    delete iter->second;
    data.changes.erase(iter);
  }
  return -1;
}


Pattern::NoteMap& Pattern::getNotes() {
  return notes;
}


const Pattern::NoteMap& Pattern::getNotes() const {
  return notes;
}


Pattern::CCData& Pattern::getCC(int ccNumber) {
  assert(ccNumber >= 0 && ccNumber < 128);
  return controlChanges[ccNumber];
}


int Pattern::getSteps() const {
  return steps;
}


int Pattern::getCCSteps() const {
  return ccSteps;
}


int Pattern::getLength() const {
  return length;
}
  

void Pattern::getDirtyRect(int* minStep, int* minNote, 
			   int* maxStep, int* maxNote) {
  if (minStep)
    *minStep = this->minStep;
  if (minNote)
    *minNote = this->minNote;
  if (maxStep)
    *maxStep = this->maxStep;
  if (maxNote)
    *maxNote = this->maxNote;
  this->minStep = length * steps;
  this->maxStep = -1;
  this->minNote = 128;
  this->maxNote = -1;
}


bool Pattern::getNextNote(int& step, int& value,int& length, 
			  int beforeStep) const {
  // no notes left in the pattern
  if (!nextNote)
    return false;
  
  // this is the first call since a findNextNote(), so use the current pointer
  if (mFirstNote && nextNote->step < beforeStep) {
    mFirstNote = false;
    step = nextNote->step;
    value = nextNote->value;
    length = nextNote->length;
    return true;
  }
  
  // this is not the first call, so go to next note and use that
  if (!mFirstNote && nextNote->next && nextNote->next->step < beforeStep) {
    nextNote = nextNote->next;
    step = nextNote->step;
    value = nextNote->value;
    length = nextNote->length;
    return true;
  }
  
  return false;
}


void Pattern::findNextNote(int step) const {
  NoteMap::const_iterator iter = notes.lower_bound(make_pair(step, 0));
  if (iter == notes.end())
    nextNote = NULL;
  else
    nextNote = iter->second;
  mFirstNote = true;
}


bool Pattern::isDirty() const {
  return mDirty;
}


void Pattern::makeClean() const {
  mDirty = false;
}


xmlNodePtr Pattern::getXMLNode(xmlDocPtr doc) const {
  xmlNodePtr node = xmlNewDocNode(doc, NULL, xmlCharStrdup("pattern"), NULL);
  char tmpStr[20];
  sprintf(tmpStr, "%d", getLength());
  xmlNewProp(node, xmlCharStrdup("length"), xmlCharStrdup(tmpStr));
  sprintf(tmpStr, "%d", getSteps());
  xmlNewProp(node, xmlCharStrdup("steps"), xmlCharStrdup(tmpStr));
  sprintf(tmpStr, "%d", getCCSteps());
  xmlNewProp(node, xmlCharStrdup("ccsteps"), xmlCharStrdup(tmpStr));
  for (NoteMap::const_iterator iter = notes.begin(); iter != notes.end();
       ++iter) {
    xmlNodePtr note = xmlNewDocNode(doc, NULL, xmlCharStrdup("note"), NULL);
    sprintf(tmpStr, "%d", iter->first.first);
    xmlNewProp(note, xmlCharStrdup("step"), xmlCharStrdup(tmpStr));
    sprintf(tmpStr, "%d", iter->first.second);
    xmlNewProp(note, xmlCharStrdup("value"), xmlCharStrdup(tmpStr));
    sprintf(tmpStr, "%d", iter->second->length);
    xmlNewProp(note, xmlCharStrdup("length"), xmlCharStrdup(tmpStr));
    xmlAddChild(node, note);
  }
  return node;
}
