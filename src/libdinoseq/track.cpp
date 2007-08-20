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

#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "controller_numbers.hpp"
#include "debug.hpp"
#include "deleter.hpp"
#include "interpolatedevent.hpp"
#include "midibuffer.hpp"
#include "pattern.hpp"
#include "track.hpp"


using namespace sigc;
using namespace xmlpp;


namespace Dino {


  Track::SequenceIterator::SequenceIterator() : m_vector(0) {

  }
  
  
  Track::SequenceEntry* Track::SequenceIterator::operator->() {
    return *m_iter;
  }
  
  
  Track::SequenceEntry& Track::SequenceIterator::operator*() {
    return **m_iter;
  }
  
  
  bool Track::SequenceIterator::operator==(const SequenceIterator& iter) const {
    return (m_iter == iter.m_iter);
  }
  
  
  bool Track::SequenceIterator::operator!=(const SequenceIterator& iter) const {
    return (m_iter != iter.m_iter);
  }
  
  
  Track::SequenceIterator& Track::SequenceIterator::operator++() {
    const SequenceEntry* old_ptr = *m_iter;
    do {
      ++m_iter;
    } while (m_iter != m_vector->end() && 
             (*m_iter == old_ptr || *m_iter == 0));
    return *this;
  }
  
  
  Track::SequenceIterator Track::SequenceIterator::operator++(int) {
    SequenceIterator iter = *this;
    ++(*this);
    return iter;
  }
  

  Track::SequenceIterator::SequenceIterator(const 
                                            std::vector<SequenceEntry*>::
                                            const_iterator& 
                                            iter,
                                            const std::vector<SequenceEntry*>&
                                            vec) 
    : m_iter(iter),
      m_vector(&vec) {
    
  }


  Track::ConstPatternIterator::ConstPatternIterator() {

  }
  
  
  const Pattern* Track::ConstPatternIterator::operator->() const {
    return m_iter->second;
  }
  
  
  const Pattern& Track::ConstPatternIterator::operator*() const {
    return *m_iter->second;
  }

  
  bool Track::ConstPatternIterator::operator==(const ConstPatternIterator& iter)
    const {
    return (m_iter == iter.m_iter);
  }
  
  
  bool Track::ConstPatternIterator::operator!=(const ConstPatternIterator& iter)
    const {
    return (m_iter != iter.m_iter);
  }
  
  
  Track::ConstPatternIterator& Track::ConstPatternIterator::operator++() {
    ++m_iter;
    return *this;
  }
  
  
  Track::ConstPatternIterator Track::ConstPatternIterator::operator++(int) {
    ConstPatternIterator iter = *this;
    ++(*this);
    return iter;
  }

  
  Track::ConstPatternIterator::ConstPatternIterator(const PatternIterator& iter)
    : m_iter(iter.m_iter) {

  }
  
  
  Track::ConstPatternIterator::
  ConstPatternIterator(const std::map<int, Pattern*>::const_iterator& iter)
    : m_iter(iter) {

  }


  Track::PatternIterator::PatternIterator() {

  }
  
  
  const Pattern* Track::PatternIterator::operator->() const {
    return m_iter->second;
  }
  
  
  const Pattern& Track::PatternIterator::operator*() const {
    return *m_iter->second;
  }
  
  
  Pattern* Track::PatternIterator::operator->() {
    return m_iter->second;
  }
  
  
  Pattern& Track::PatternIterator::operator*() {
    return *m_iter->second;
  }
  
  
  bool Track::PatternIterator::operator==(const PatternIterator& iter) const {
    return (m_iter == iter.m_iter);
  }
  
  
  bool Track::PatternIterator::operator!=(const PatternIterator& iter) const {
    return (m_iter != iter.m_iter);
  }
  
  
  Track::PatternIterator& Track::PatternIterator::operator++() {
    ++m_iter;
    return *this;
  }
      

  Track::PatternIterator Track::PatternIterator::operator++(int) {
    PatternIterator iter = *this;
    ++(*this);
    return iter;
  }


  Track::PatternIterator::
  PatternIterator(const std::map<int, Pattern*>::iterator& iter)
    : m_iter(iter) {

  }


  Track::Track(int id, int length, const string& name) 
    : m_id(id),
      m_name(name),
      m_next_sid(0),
      m_sequence(new vector<SequenceEntry*>(length, (SequenceEntry*)0)),
      m_curves(new vector<Curve*>),
      m_dirty(false) {
  
    dbg1<<"Creating track \""<<name<<"\""<<endl;
  }


  Track::~Track() {
    dbg1<<"Destroying track \""<<m_name<<"\""<<endl;
    map<int, Pattern*>::iterator iter;
    for (iter = m_patterns.begin(); iter != m_patterns.end(); ++iter)
      delete iter->second;
    SequenceEntry* ptr = 0;
    for (unsigned int i = 0; i < m_sequence->size(); ++i) {
      if ((*m_sequence)[i] != ptr)
        delete (*m_sequence)[i];
      ptr = (*m_sequence)[i];
    }
    delete m_sequence;
  }


  int Track::get_id() const {
    return m_id;
  }
  

  /** Returns the name of the track. The name is just a label that the user
      can change. */
  const string& Track::get_name() const {
    return m_name;
  }

  
  const string& Track::get_label() const {
    return m_name;
  }

  
  Track::ConstPatternIterator Track::pat_begin() const {
    return ConstPatternIterator(m_patterns.begin());
  }
  
  
  Track::ConstPatternIterator Track::pat_end() const {
    return ConstPatternIterator(m_patterns.end());
  }
  
  
  Track::ConstPatternIterator Track::pat_find(int id) const {
    return ConstPatternIterator(m_patterns.find(id));
  }


  Track::PatternIterator Track::pat_begin() {
    return PatternIterator(m_patterns.begin());
  }
  
  
  Track::PatternIterator Track::pat_end() {
    return PatternIterator(m_patterns.end());
  }
  
  
  Track::PatternIterator Track::pat_find(int id) {
    return PatternIterator(m_patterns.find(id));
  }


  Track::SequenceIterator Track::seq_begin() const {
    std::vector<SequenceEntry*>::const_iterator iter;
    unsigned i;
    for (i = 0; i < m_sequence->size(); ++i) {
      if ((*m_sequence)[i] != 0)
        break;
    }
    return SequenceIterator(m_sequence->begin() + i, *m_sequence);
  }
  
  
  Track::SequenceIterator Track::seq_end() const {
    return SequenceIterator(m_sequence->end(), *m_sequence);
  }
  
  
  Track::SequenceIterator Track::seq_find(unsigned int beat) const {
    if ((*m_sequence)[beat] != 0)
      return SequenceIterator(m_sequence->begin() + beat, *m_sequence);
    return SequenceIterator(m_sequence->end(), *m_sequence);
  }


  Track::SequenceIterator Track::seq_find_by_id(int id) const {
    for (unsigned int beat = 0; beat < m_sequence->size(); ++beat) {
      if ((*m_sequence)[beat] != 0) {
        if ((*m_sequence)[beat]->get_id() == id)
          return SequenceIterator(m_sequence->begin() + beat, *m_sequence);
      }
    }
    return SequenceIterator(m_sequence->end(), *m_sequence);
  }


  Track::ConstCurveIterator Track::curves_begin() const {
    return ConstCurveIterator(m_curves->begin());
  }
  
  
  Track::ConstCurveIterator Track::curves_end() const {
    return ConstCurveIterator(m_curves->end());
  }
  
  
  Track::ConstCurveIterator Track::curves_find(long param) const {
    for (unsigned i = 0; i < m_curves->size(); ++i) {
      if ((*m_curves)[i]->get_info().get_number() == param)
        return ConstCurveIterator(m_curves->begin() + i);
    }
    return curves_end();
  }

  
  Track::CurveIterator Track::curves_begin() {
    return CurveIterator(m_curves->begin());
  }
  
  
  Track::CurveIterator Track::curves_end() {
    return CurveIterator(m_curves->end());
  }
  
  
  Track::CurveIterator Track::curves_find(long param) {
    for (unsigned i = 0; i < m_curves->size(); ++i) {
      if ((*m_curves)[i]->get_info().get_number() == param)
        return CurveIterator(m_curves->begin() + i);
    }
    return curves_end();
  }

  
  /** Returns the MIDI channel for this track. */
  int Track::get_channel() const {
    return m_channel;
  }


  unsigned int Track::get_length() const {
    return m_sequence->size();
  }


  const std::vector<ControllerInfo*>& Track::get_controllers() const {
    return m_controllers;
  }
  
  
  std::vector<ControllerInfo*>& Track::get_controllers() {
    return m_controllers;
  }
  
  
  Curve* Track::get_curve(long number) {
    return 0;
  }


  bool Track::add_controller(long number, const std::string& name, 
			     int default_v, int min, int max, bool global) {
    
    // check if we already have this controller, if so return false
    for (int i = 0; i < m_controllers.size(); ++i) {
      if (m_controllers[i]->get_number() == number)
	return false;
    }
    
    // add the ControllerInfo object
    dbg1<<"Adding controller \""<<name<<"\" in track \""<<m_name<<"\""<<endl;
    ControllerInfo* ci = new ControllerInfo(number, name);
    ci->set_default(default_v);
    ci->set_min(min);
    ci->set_max(max);
    ci->set_global(global);
    m_controllers.push_back(ci);
    
    // if this is not a global controller, add curves to all patterns
    if (!global) {
      PatternIterator pi;
      for (pi = pat_begin(); pi != pat_end(); ++pi)
	pi->add_curve(*ci);
    }
    
    // else (if it is a global controller), add a curve to the track
    else {
      m_curves->push_back(new Curve(*ci, get_length()));
      m_signal_curve_added(number);
    }
    
    m_signal_controller_added(number);
    
    return true;
  }


  bool Track::add_controller(ControllerInfo* info, map<int, Curve*>& curves) {

    // check if we already have this controller, if so return false
    for (int i = 0; i < m_controllers.size(); ++i) {
      if (m_controllers[i]->get_number() == info->get_number())
	return false;
    }
    
    // add the ControllerInfo object
    dbg1<<"Adding controller \""<<info->get_name()<<"\" in track \""
	<<m_name<<"\""<<endl;
    m_controllers.push_back(info);
    
    // if this is not a global controller, add curves to all patterns
    if (!info->get_global()) {
      PatternIterator pi;
      for (pi = pat_begin(); pi != pat_end(); ++pi) {
	map<int, Curve*>::iterator citer = curves.find(pi->get_id());
	if (citer == curves.end())
	  pi->add_curve(*info);
	else {
	  assert(citer->second->get_size() == 
		 (pi->get_length() * pi->get_steps()));
	  pi->add_curve(citer->second);
	}
      }
    }
    
    // else (if it is a global controller), add a curve to the track
    else {
      map<int, Curve*>::iterator citer = curves.find(-1);
      if (citer == curves.end())
	m_curves->push_back(new Curve(*info, get_length()));
      else {
	assert(citer->second->get_size() == get_length());
	m_curves->push_back(citer->second);
      }
      m_signal_curve_added(info->get_number());
    }
    
    m_signal_controller_added(info->get_number());
    
    return true;
  }
  
  
  bool Track::remove_controller(long number) {
    
    map<int, Curve*> curves;
    ControllerInfo* info = disown_controller(number, curves);
    
    if (!info)
      return false;
    
    Deleter::queue(info);
    
    map<int, Curve*>::iterator iter;
    for (iter = curves.begin(); iter != curves.end(); ++iter)
      Deleter::queue(iter->second);
    
    return true;
  }


  ControllerInfo* Track::disown_controller(long number, 
					   map<int, Curve*>& curves) {

    // check if it exists
    for (int i = 0; i < m_controllers.size(); ++i) {
      if (m_controllers[i]->get_number() == number) {
	
	// if this is not a global controller, remove all curves from patterns
	if (!m_controllers[i]->get_global()) {
	  PatternIterator pi;
	  for (pi = pat_begin(); pi != pat_end(); ++pi) {
	    Curve* c = pi->disown_curve(pi->curves_find(m_controllers[i]->
							get_number()));
	    if (c)
	      curves[pi->get_id()] = c;
	  }
	}
	
	// else (if it is a global controller), remove the curve from the track
	else {
	  unsigned j;
	  for (j = 0; i < m_curves->size(); ++i) {
	    if ((*m_curves)[j]->get_info().get_number() == number)
	      break;
	  }
	  assert(j < m_curves->size());
	  if (j < m_curves->size()) {
	    Curve* tmp_curve = (*m_curves)[j];
	    m_curves->erase(m_curves->begin() + j);
	    curves[-1] = tmp_curve;
	    m_signal_curve_removed(number);
	  }
	}

	ControllerInfo* tmp = m_controllers[i];
	dbg1<<"Removing controller \""<<tmp->get_name()<<"\" from track \""
	    <<m_name<<"\""<<endl;
	m_controllers.erase(m_controllers.begin() + i);
	m_signal_controller_removed(number);

	return tmp;
      }
    }
    
    return 0;
  }

  
  void Track::set_controller_name(long number, const std::string& name) {
    
    // XXX need a controller_find() function
    unsigned i;
    for (i = 0; i < m_controllers.size(); ++i)
      if (m_controllers[i]->get_number() == number)
	break;
    if (i == m_controllers.size())
      return;
    
    if (m_controllers[i]->get_name() == name)
      return;
    
    m_controllers[i]->set_name(name);
    
    m_signal_controller_changed(number);
    
  }


  void Track::set_controller_min(long number, int min) {
    
    // XXX need a controller_find() function
    unsigned i;
    for (i = 0; i < m_controllers.size(); ++i)
      if (m_controllers[i]->get_number() == number)
	break;
    if (i == m_controllers.size())
      return;
    
    if (m_controllers[i]->get_min() == min)
      return;
    
    m_controllers[i]->set_min(min);
    
    m_signal_controller_changed(number);
        
  }


  void Track::set_controller_max(long number, int max) {

    // XXX need a controller_find() function
    unsigned i;
    for (i = 0; i < m_controllers.size(); ++i)
      if (m_controllers[i]->get_number() == number)
	break;
    if (i == m_controllers.size())
      return;
    
    if (m_controllers[i]->get_max() == max)
      return;
    
    m_controllers[i]->set_max(max);
    
    m_signal_controller_changed(number);
    
  }


  void Track::set_controller_default(long number, int _default) {

    // XXX need a controller_find() function
    unsigned i;
    for (i = 0; i < m_controllers.size(); ++i)
      if (m_controllers[i]->get_number() == number)
	break;
    if (i == m_controllers.size())
      return;
    
    if (m_controllers[i]->get_default() == _default)
      return;
    
    m_controllers[i]->set_default(_default);
    
    m_signal_controller_changed(number);
    
  }


  void Track::set_controller_number(long number, long new_number) {

    // XXX need a controller_find() function
    unsigned i;
    for (i = 0; i < m_controllers.size(); ++i)
      if (m_controllers[i]->get_number() == number)
	break;
    if (i == m_controllers.size())
      return;
    
    if (m_controllers[i]->get_number() == new_number)
      return;
    
    m_controllers[i]->set_number(new_number);
    
    m_signal_controller_changed(number);
    
  }


  void Track::set_controller_global(long number, bool global) {
    
    unsigned i;
    for (i = 0; i < m_controllers.size(); ++i)
      if (m_controllers[i]->get_number() == number)
	break;
    if (i == m_controllers.size())
      return;
    
    bool old_global = m_controllers[i]->get_global();
    
    if (old_global == global)
      return;
    
    m_controllers[i]->set_global(global);
    
    // it was global, will now be non-global
    if (!global) {
      
      // remove the global curve
      // XXX should add a private remove_curve() function
      unsigned j;
      for (j = 0; i < m_curves->size(); ++i) {
	if ((*m_curves)[j]->get_info().get_number() == number)
	  break;
      }
      assert(j < m_curves->size());
      if (j < m_curves->size()) {
	Curve* tmp_curve = (*m_curves)[j];
	m_curves->erase(m_curves->begin() + j);
	Deleter::queue(tmp_curve);
	m_signal_curve_removed(number);
      }	
      
      // add pattern curves
      PatternIterator pi;
      for (pi = pat_begin(); pi != pat_end(); ++pi)
	pi->add_curve(*m_controllers[i]);
    }
    
    // it was non-global, will now be global
    else {
      
      // remove all pattern curves
      PatternIterator pi;
      for (pi = pat_begin(); pi != pat_end(); ++pi)
	pi->remove_curve(pi->curves_find(number));
      
      // add global curve
      // XXX should add a private add_curve() function
      m_curves->push_back(new Curve(*m_controllers[i], get_length()));
      m_signal_curve_added(number);
      
    }
    
    m_signal_controller_changed(number);
  }
  
    
  /** Sets the name of this track. */
  void Track::set_name(const string& name) {
    if (name != m_name) {
      dbg1<<"Changing track name from \""<<m_name<<"\" to \""<<name<<"\""<<endl;
      m_name = name;
      m_signal_name_changed(m_name);
    }
  }


  /** Creates and adds a new pattern in this track with the given parameters.*/
  Track::PatternIterator Track::add_pattern(const string& name, int length, 
                                            int steps) {
    /* This does not actually need to be threadsafe since the sequencer
       never accesses the patterns through the map, only through the 
       sequence entries. */
    int id;
    if (m_patterns.rbegin() != m_patterns.rend())
      id = m_patterns.rbegin()->first + 1;
    else
      id = 1;
    Pattern* pat = new Pattern(id, name, length, steps);
    return add_pattern(pat);
  }


  Track::PatternIterator Track::add_pattern(Pattern* pattern) {
    int id = pattern->get_id();
    if (m_patterns.find(id) != m_patterns.end())
      return pat_end();
    m_patterns[id] = pattern;
    m_signal_pattern_added(id);
    
    // add curves for all non-global controllers
    for (unsigned i = 0; i < m_controllers.size(); ++i) {
      if (!m_controllers[i]->get_global())
	m_patterns[id]->add_curve(*m_controllers[i]);
    }
    
    return PatternIterator(m_patterns.find(id));
  }


  Track::PatternIterator Track::duplicate_pattern(ConstPatternIterator iter) {
    /* This does not need to be threadsafe since the sequencer thread
       never accesses the patterns through the map, only through the sequencer
       entries. */
    
    if (iter == pat_end())
      return pat_end();
    
    int id;
    if (m_patterns.rbegin() != m_patterns.rend())
      id = m_patterns.rbegin()->first + 1;
    else
      id = 1;
    m_patterns[id] = new Pattern(id, *iter.m_iter->second);
    m_signal_pattern_added(id);
    return PatternIterator(m_patterns.find(id));
  }

  
  /** Removes the pattern with the given ID. */
  void Track::remove_pattern(int id) {
    Pattern* pat = disown_pattern(id);
    Deleter::queue(pat);
  }
  

  // XXX must figure out what to do with the ControllerInfo references here -
  //     it's probably not a good idea to let the curves store them by reference
  Pattern* Track::disown_pattern(int id) {
    /* Changing the map itself does not need to be threadsafe since the 
       sequencer never accesses patterns through the map, but actually
       deleting the pattern must be done in a threadsafe way. */
    map<int, Pattern*>::iterator iter = m_patterns.find(id);
    if (iter == m_patterns.end())
      return 0;
    for (unsigned int i = 0; i < m_sequence->size(); ++i) {
      if ((*m_sequence)[i] != 0 && (*m_sequence)[i]->pattern == iter->second)
	remove_sequence_entry(seq_find(i));
    }
    
    Pattern* result = iter->second;
    m_patterns.erase(iter);
    m_signal_pattern_removed(id);
    
    return result;
  }



  /** Set the sequency entry at the given beat to the given pattern 
      and length. */
  Track::SequenceIterator Track::set_sequence_entry(int beat, 
                                                    int pattern, 
                                                    unsigned int length) {
    // XXX shouldn't exit here, just return an invalid iterator
    assert(beat >= 0);
    assert(beat < int(m_sequence->size()));
    assert(m_patterns.find(pattern) != m_patterns.end());
    assert(length >= 0);
    assert(length <= m_patterns.find(pattern)->second->get_length());
    
    // if length is 0, get it from the pattern
    int newLength;
    if (length == 0)
      newLength = m_patterns[pattern]->get_length();
    else
      newLength = length;
  
    // delete or shorten any sequence entry at this beat
    SequenceEntry* se = (*m_sequence)[beat];
    if (se) {
      int stop = se->start + se->length;
      for (int i = stop - 1; i >= beat; --i)
        (*m_sequence)[i] = 0;
      if (se->start == unsigned(beat))
        Deleter::queue(se);
      else
        se->length = beat - se->start;
    }
  
    // make sure the new sequence entry fits
    for (int i = beat; i < beat + newLength; ++i) {
      if (i >= int(m_sequence->size()) || (*m_sequence)[i]) {
        newLength = i - beat;
        break;
      }
    }
  
    se  = new SequenceEntry(pattern, m_patterns.find(pattern)->second, 
                            beat, newLength);
    se->id = m_next_sid;
    cerr<<"Setting sid for new seq to "<<m_next_sid<<endl;
    ++m_next_sid;
    if (m_next_sid == -1)
      ++m_next_sid;
    
    int i;
    for (i = beat; i < beat + newLength; ++i)
      (*m_sequence)[i] = se;
    
    m_signal_sequence_entry_added(beat, (*m_sequence)[beat]->pattern->get_id(), 
                                  newLength);
    return SequenceIterator(m_sequence->begin() + i, *m_sequence);
  }


  void Track::set_seq_entry_length(SequenceIterator iter, unsigned int length) {
    unsigned beat = iter->start;
    assert((*m_sequence)[beat]);
    SequenceEntry* se = (*m_sequence)[beat];
    if (length == se->length)
      return;
    else if (length > se->length) {
      unsigned int i;
      for (i = se->start + se->length; i < se->start + length; ++i) {
        if ((*m_sequence)[i])
          break;
        (*m_sequence)[i] = se;
      }
      se->length = i - se->start;
    }
    else {
      int tmp = se->length;
      se->length = length;
      for (int i = se->start + tmp - 1; i >= int(se->start + se->length); --i)
        (*m_sequence)[i] = 0;
    }
    m_signal_sequence_entry_changed(beat, se->pattern->get_id(), se->length);
  }



  /** Remove the sequence entry (pattern) that is playing at the given beat.
      If no pattern is playing at that beat, return @c false. */
  bool Track::remove_sequence_entry(SequenceIterator iterator) {
    if (iterator == seq_end())
      return false;
    
    unsigned beat = iterator->start;
    SequenceEntry* se = (*m_sequence)[beat];
    if (se) {
      int start = se->start;
      for (int i = se->start + se->length - 1; i >= int(se->start); --i)
        (*m_sequence)[i] = 0;
      Deleter::queue(se);
      m_signal_sequence_entry_removed(start);
      return true;
    }
    return false;
  }


  /** Set the length of the track. Only the Song should do this. */
  void Track::set_length(int length) {
    
    assert(length > 0);
    if (length != int(m_sequence->size())) {
      for (unsigned i = length; i < m_sequence->size(); ++i) {
        if ((*m_sequence)[i]) {
          if ((*m_sequence)[i]->start < (unsigned)length) {
            set_seq_entry_length(seq_find((*m_sequence)[i]->start), 
                                 length - (*m_sequence)[i]->start);
          }
          else {
            remove_sequence_entry(seq_find(i));
          }
        }
      }
      
      vector<SequenceEntry*>* new_seq = new vector<SequenceEntry*>(*m_sequence);
      new_seq->resize(length);
      vector<SequenceEntry*>* old_seq = m_sequence;
      m_sequence = new_seq;
      Deleter::queue(old_seq);
      
      m_signal_length_changed(m_sequence->size());
    }
  }


  /** Set the MIDI channel for this track. */
  void Track::set_channel(int channel) {
    dbg1<<"Setting MIDI channel for track \""<<m_name<<"\" to "<<channel<<endl;
    m_channel = channel;
  }


  /** Has the track been changed since the last makeClean()? */
  bool Track::is_dirty() const {
    if (m_dirty)
      return true;
    for (map<int, Pattern*>::const_iterator iter = m_patterns.begin();
         iter != m_patterns.end(); ++iter) {
      if (iter->second->is_dirty())
        return true;
    }
    return false;
  }

  /** Reset the dirty flag. */
  void Track::make_clean() const {
    m_dirty = false;
    for (map<int, Pattern*>::const_iterator iter = m_patterns.begin();
         iter != m_patterns.end(); ++iter)
      iter->second->make_clean();
  }


  /** Serialize this track to a XML node and return it. */
  bool Track::fill_xml_node(Element* elt) const {
  
    // MIDI channel
    elt->set_attribute("name", m_name);
    char tmp_txt[10];
    sprintf(tmp_txt, "%d", get_channel());
    elt->set_attribute("channel", tmp_txt);
 
    // the patterns
    for (map<int, Pattern*>::const_iterator iter = m_patterns.begin();
         iter != m_patterns.end(); ++iter) {
      Element* pat_elt = elt->add_child("pattern");
      char id_txt[10];
      sprintf(id_txt, "%d", iter->first);
      pat_elt->set_attribute("id", id_txt);
      iter->second->fill_xml_node(pat_elt);
    }
  
    // the sequence
    Element* seq_elt = elt->add_child("sequence");
    for (unsigned int i = 0; i < m_sequence->size(); ++i) {
      SequenceEntry* se = (*m_sequence)[i];
      if (se && se->start == i) {
        Element* entry_elt = seq_elt->add_child("entry");
        sprintf(tmp_txt, "%d", se->start);
        entry_elt->set_attribute("beat", tmp_txt);
        sprintf(tmp_txt, "%d", se->pattern_id);
        entry_elt->set_attribute("pattern", tmp_txt);
        sprintf(tmp_txt, "%d", se->length);
        entry_elt->set_attribute("length", tmp_txt);
      }
    }
  
    return true;
  }


  bool Track::parse_xml_node(const Element* elt) {
    Node::NodeList nodes;
    Node::NodeList::const_iterator iter;
  
    // MIDI channel
    int channel;
    sscanf(elt->get_attribute("channel")->get_value().c_str(), 
           "%d", &channel);
    m_channel = channel;
    m_name = elt->get_attribute("name")->get_value();
  
    // parse patterns
    nodes = elt->get_children("pattern");
    for (iter = nodes.begin(); iter != nodes.end(); ++iter) {
      const Element* pat_elt = dynamic_cast<const Element*>(*iter);
      if (!pat_elt)
        continue;
      int id, length, steps;
      sscanf(pat_elt->get_attribute("id")->get_value().c_str(), "%d", &id);
      sscanf(pat_elt->get_attribute("length")->get_value().c_str(), 
             "%d", &length);
      sscanf(pat_elt->get_attribute("steps")->get_value().c_str(), 
             "%d", &steps);
      string name = pat_elt->get_attribute("name")->get_value();
      m_patterns[id] = new Pattern(id, name, length, steps);
      m_patterns[id]->parse_xml_node(pat_elt);
    }
  
    // parse sequence
    nodes = elt->get_children("sequence");
    if (nodes.begin() != nodes.end()) {
      const Element* seq_elt = dynamic_cast<const Element*>(*nodes.begin());
      if (seq_elt) {
        Node::NodeList e_nodes = (*nodes.begin())->get_children("entry");
        Node::NodeList::const_iterator e_iter;
        for (e_iter = e_nodes.begin(); e_iter != e_nodes.end(); ++e_iter) {
          const Element* entry_elt = dynamic_cast<const Element*>(*e_iter);
          if (!entry_elt)
            continue;
          int beat, pattern, length;
          sscanf(entry_elt->get_attribute("beat")->get_value().c_str(), 
                 "%d", &beat);
          sscanf(entry_elt->get_attribute("pattern")->get_value().c_str(), 
                 "%d", &pattern);
          sscanf(entry_elt->get_attribute("length")->get_value().c_str(), 
                 "%d", &length);
          set_sequence_entry(beat, pattern, length);
        }
      }
    }
    return true;
  }


  void Track::sequence(MIDIBuffer& buffer, double from, double to,
		       unsigned int length, int channel) const {
    assert(from >= 0);
    assert(to >= 0);
    
    const vector<SequenceEntry*>& sequence = *m_sequence;
    if (from >= sequence.size())
      return;
    if (to > sequence.size())
      to = sequence.size();

    std::vector<Curve*>& curves = *m_curves;
    bool sequence_globals = false;
    if (curves.size() > 0 && curves[0]->get_size() == sequence.size())
      sequence_globals = true;
    
    //dbg1<<"curves.size() == "<<curves.size()<<endl;
    //dbg1<<"curves[0]->get_size() == "<<curves[0]->get_size()<<endl;
    //dbg1<<"sequence_globals = "<<sequence_globals<<endl;
    
    // iterate over all beats in the interval [from, to)
    for ( ; from < to; from = floor(from + 1)) {
      
      // write global controller events
      if (sequence_globals) {
	buffer.set_offset(0);
	for (unsigned c = 0; c < curves.size(); ++c) {
	  const InterpolatedEvent* event = curves[c]->get_event(unsigned(from));
          if (event) {
	    //dbg1<<"Sequencing event for curve "<<c<<endl;
            unsigned char* data = buffer.reserve(from, 3);
            if (data && is_cc(curves[c]->get_info().get_number())) {
              data[0] = 0xB0 | (unsigned char)m_channel;
              data[1] = cc_number(curves[c]->get_info().get_number());
              data[2] = (unsigned char)
                (event->get_start() + (from - event->get_step()) *
                 ((event->get_end() - event->get_start()) /
                  double(event->get_length())));
            }
            else if (data && is_pbend(curves[c]->get_info().get_number())) {
	      //dbg1<<"Pitchbend event"<<endl;
              data[0] = 0xE0 | (unsigned char)m_channel;
              int value = int(event->get_start() + 
                              (from - event->get_step()) *
                              ((event->get_end() - event->get_start()) /
                               double(event->get_length())));
              data[1] = (value + 8192) & 0x7F;
              data[2] = ((value + 8192) >> 7) & 0x7F;
            }
          }
	}
      }
      
      // if we're in a pattern, sequence it
      if (sequence[unsigned(from)]) {
        SequenceEntry* const& se = sequence[unsigned(from)];
	buffer.set_offset(se->start);
	double local_to = to < from + 1 ? to : from + 1;
	local_to -= se->start;
        se->pattern->sequence(buffer, from - se->start, 
                              local_to, se->length, m_channel);
        //beat += sequence[beat]->start + sequence[beat]->length - beat;
      }
      
      // else, send all notes off
      else {
        unsigned char all_notes_off[] = { 0xB0, 123, 0 };
        unsigned char* data = buffer.reserve(from, 3);
        if (data) {
          memcpy(data, all_notes_off, 3);
          data[0] |= (unsigned char)m_channel;
        }
      }
      
    }

  }


  signal<void, const string&>& Track::signal_name_changed() const {
    return m_signal_name_changed;
  }


  signal<void, int>& Track::signal_pattern_added() const {
    return m_signal_pattern_added;
  }


  signal<void, int>& Track::signal_pattern_removed() const {
    return m_signal_pattern_removed;
  }


  signal<void, int, int, int>& Track::signal_sequence_entry_added() const {
    return m_signal_sequence_entry_added;
  }


  signal<void, int, int, int>& Track::signal_sequence_entry_changed() const {
    return m_signal_sequence_entry_changed;
  }


  signal<void, int>& Track::signal_sequence_entry_removed() const {
    return m_signal_sequence_entry_removed;
  }


  signal<void, int>& Track::signal_length_changed() const {
    return m_signal_length_changed;
  }


  signal<void, long>& Track::signal_controller_added() const {
    return m_signal_controller_added;
  }

  
  signal<void, long>& Track::signal_controller_removed() const {
    return m_signal_controller_removed;
  }
  
  
  signal<void, long>& Track::signal_controller_changed() const {
    return m_signal_controller_changed;
  }
  
  
  signal<void, long>& Track::signal_curve_added() const {
    return m_signal_curve_added;
  }
  
  
  signal<void, long>& Track::signal_curve_removed() const {
    return m_signal_curve_removed;
  }


}
