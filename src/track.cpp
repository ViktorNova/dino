#include <cstdio>
#include <iostream>

#include "track.hpp"


Track::Track(int length) 
  : mLength(length), mDirty(false), currentSeqEntry(NULL) {

}


const string& Track::getName() const {
  return mName;
}


map<int, Pattern>& Track::getPatterns() {
  return mPatterns;
}


const map<int, Pattern>& Track::getPatterns() const {
  return mPatterns;
}


Track::Sequence& Track::getSequence() {
  return mSequence;
}


bool Track::getSequenceEntry(int at, int& beat, int& pattern, int& length) {
  for (Sequence::const_iterator iter = mSequence.begin(); 
       iter != mSequence.end() && iter->first <= at; ++iter) {
    if (iter->first + iter->second->length > at) {
      beat = iter->first;
      pattern = iter->second->patternID;
      length = iter->second->length;
      return true;
    }
  }
  return false;
}


int Track::addPattern(int length, int steps, int ccSteps) {
  int id;
  if (mPatterns.rbegin() != mPatterns.rend())
    id = mPatterns.rbegin()->first + 1;
  else
    id = 1;
  mPatterns[id] = Pattern(length, steps, ccSteps);
  return id;
}


void Track::setSequenceEntry(int beat, int pattern, int length) {
  
  assert(beat >= 0);
  assert(beat < mLength);
  assert(mPatterns.find(pattern) != mPatterns.end());
  assert(length > 0 || length == -1);
  assert(length <= mPatterns.find(pattern)->second.getLength());
  
  // if length is -1, get it from the pattern
  int newLength;
  if (length == -1)
    newLength = mPatterns[pattern].getLength();
  else
    newLength = length;
  
  SequenceEntry* previous = NULL;
  SequenceEntry* next = NULL;
  
  Sequence::iterator iter;
  Sequence::iterator toUpdate = mSequence.end();
  for (iter = mSequence.begin(); iter != mSequence.end(); ++iter) {
    
    // if a pattern is playing at the given beat, shorten it so it stops just
    // before that beat
    if (iter->first < beat) {
      previous = iter->second;
      if (iter->first + iter->second->length > beat)
	iter->second->length = beat - iter->first;
    }
    // if there already is a pattern starting at this beat, erase it unless 
    // it's the same pattern, in which case we just remember it for later
    else if (iter->first == beat) {
      if (iter->second->patternID == pattern)
	toUpdate = iter;
      else
	mSequence.erase(iter);
    }
    
    // we've found the next sequence entry - remember it as "next" and stop
    // also make sure that the new sequence entry doesn't overlap with this one
    else if (iter->first > beat) {
      next = iter->second;
      if (iter->first < beat + newLength)
	newLength = iter->first - beat;
      break;
    }
  }
  
  // force the pattern to stop at the end of the song
  if (beat + newLength > mLength)
    newLength = mLength - beat;
  
  // if the pattern was there already, just update the length
  if (toUpdate != mSequence.end()) {
    toUpdate->second->length = newLength;
    return;
  }
  
  else {
    // setup the linked list and add the new entry
    SequenceEntry* entry = new SequenceEntry(pattern, 
					     &mPatterns.find(pattern)->second, 
					     beat, newLength);
    entry->next = next;
    entry->previous = previous;
    if (next)
      next->previous = entry;
    if (previous)
      previous->next = entry;
    mSequence[beat] = entry;
  }
}


bool Track::removeSequenceEntry(int beat) {
  Sequence::iterator iter = mSequence.begin();
  for ( ; iter != mSequence.end(); ++iter) {
    if (iter->first <= beat && iter->first + iter->second->length > beat) {
      if (iter->second->previous)
	iter->second->previous->next = iter->second->next;
      if (iter->second->next)
	iter->second->next->previous = iter->second->previous;
      delete iter->second;
      mSequence.erase(iter);
      return true;
    }
  }
  return false;
}


void Track::setLength(int length) {
  mLength = length;
}


bool Track::isDirty() const {
  if (mDirty)
    return true;
  for (map<int, Pattern>::const_iterator iter = mPatterns.begin();
       iter != mPatterns.end(); ++iter) {
    if (iter->second.isDirty())
      return true;
  }
  return false;
}


void Track::makeClean() const {
  mDirty = false;
  for (map<int, Pattern>::const_iterator iter = mPatterns.begin();
       iter != mPatterns.end(); ++iter)
    iter->second.makeClean();
}


bool Track::getNextNote(int& beat, int& tick, int& value, int& length,
			int beforeBeat, int beforeTick) const {
  // no patterns left to play
  if (!currentSeqEntry)
    return false;
  
  // convert beats and ticks to steps
  const Pattern* pat = currentSeqEntry->pattern;
  int steps = pat->getSteps();
  int bStep = (beforeBeat - currentSeqEntry->start) * steps + 
    int(beforeTick * steps / 10000.0);
  int totalLength = pat->getSteps() * currentSeqEntry->length;
  bStep = (bStep < totalLength ? bStep : totalLength);
  
  // while loops are scary in SCHED_FIFO...
  int step;
  while (currentSeqEntry && 
	 !currentSeqEntry->pattern->getNextNote(step, value, length, bStep)) {
    // nothing found, and we don't want notes from next pattern
    if (currentSeqEntry->start + currentSeqEntry->length > beforeBeat)
      return false;
    // we might want notes from next pattern, if it starts early enough
    else if (currentSeqEntry->next && 
	     currentSeqEntry->next->start < beforeBeat) {
      currentSeqEntry = currentSeqEntry->next;
      if (currentSeqEntry) {
	currentSeqEntry->pattern->findNextNote(0);
	pat = currentSeqEntry->pattern;
	steps = pat->getSteps();
	bStep = (beforeBeat - currentSeqEntry->start) * steps + 
	  int(beforeTick * steps / 10000.0);
	totalLength = pat->getSteps() * currentSeqEntry->length;
	bStep = (bStep < totalLength ? bStep : totalLength);
      }
      cerr<<"Pattern switch"<<endl;
    }
    // it doesn't
    else
      return false;
  }
  
  // did we get anything?
  if (currentSeqEntry) {
    int steps = currentSeqEntry->pattern->getSteps();
    beat = currentSeqEntry->start + step / steps;
    tick = int((step % steps) * 10000.0 / steps);
    length = int(length * 10000.0 / steps);
    return true;
  }
  
  return false;
}


bool Track::getNextCCEvent(int& step, int& tick, 
			   int& number, int& value) const {
  return false;
}


void Track::findNextNote(int beat, int tick) const {
  Sequence::const_iterator iter;
  for (iter = mSequence.begin(); iter != mSequence.end(); ++iter) {
    if (iter->first <= beat && iter->first + iter->second->length > beat)
      break;
  }
  if (iter == mSequence.end())
    currentSeqEntry == NULL;
  else {
    currentSeqEntry = iter->second;
    int steps = currentSeqEntry->pattern->getSteps();
    int step = (beat - iter->first) * steps + int(tick * steps / 10000.0);
    currentSeqEntry->pattern->findNextNote(step);
  }
}


xmlNodePtr Track::getXMLNode(xmlDocPtr doc) const {
  xmlNodePtr node = xmlNewDocNode(doc, NULL, xmlCharStrdup("track"), NULL);
  char tmpStr[20];
  
  // the patterns
  for (map<int, Pattern>::const_iterator iter = mPatterns.begin();
       iter != mPatterns.end(); ++iter) {
    xmlNodePtr child = iter->second.getXMLNode(doc);
    sprintf(tmpStr, "%d", iter->first);
    xmlNewProp(child, xmlCharStrdup("id"), xmlCharStrdup(tmpStr));
    xmlAddChild(node, child);
  }
  
  // the sequence
  xmlNodePtr seq = xmlNewDocNode(doc, NULL, xmlCharStrdup("sequence"), NULL);
  xmlAddChild(node, seq);
  for (Sequence::const_iterator iter = mSequence.begin();
       iter != mSequence.end(); ++iter) {
    xmlNodePtr entry = xmlNewDocNode(doc, NULL, xmlCharStrdup("entry"), NULL);
    sprintf(tmpStr, "%d", iter->first);
    xmlNewProp(entry, xmlCharStrdup("beat"), xmlCharStrdup(tmpStr));
    sprintf(tmpStr, "%d", iter->second->patternID);
    xmlNewProp(entry, xmlCharStrdup("pattern"), xmlCharStrdup(tmpStr));
    sprintf(tmpStr, "%d", iter->second->length);
    xmlNewProp(entry, xmlCharStrdup("length"), xmlCharStrdup(tmpStr));
    xmlAddChild(seq, entry);
  }
  
  return node;
}
