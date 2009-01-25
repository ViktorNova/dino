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

#include <iostream>
#include <typeinfo>

#include "event.hpp"
#include "noteselection.hpp"


using namespace std;


namespace Dino {


  Event& NoteSelection::Iterator::operator*() {
    return const_cast<Event&>(**m_iter);
  }
  
  
  Event* NoteSelection::Iterator::operator->() {
    return const_cast<Event*>((*m_iter).operator->());
  }

  
  bool NoteSelection::Iterator::operator==(const Iterator& iter) const {
    return (m_iter == iter.m_iter);
  }
  
  
  bool NoteSelection::Iterator::operator!=(const Iterator& iter) const {
    return (m_iter != iter.m_iter);
  }
  
  
  NoteSelection::Iterator::operator Pattern::NoteIterator() const {
    return *m_iter;
  }
    
  
  NoteSelection::Iterator& NoteSelection::Iterator::operator++() {
    ++m_iter;
    return *this;
  }
  

  NoteSelection::Iterator NoteSelection::Iterator::operator++(int) {
    Iterator result = *this;
    ++(*this);
    return result;
  }
  
    
  NoteSelection::Iterator::
  Iterator(const std::set<Pattern::NoteIterator>::iterator& iterator) 
    : m_iter(iterator) {

  }
  
  
  NoteSelection::NoteSelection(const Pattern* pat) 
    : m_pat(pat) {
    if (m_pat) {
      m_pat->signal_note_removed().
	connect(mem_fun(*this, &NoteSelection::remove_note_internal));
    }
  }

  
  NoteSelection::NoteSelection(const NoteSelection& sel) 
    : m_data(sel.m_data),
      m_pat(sel.m_pat) {
    if (m_pat)
      m_pat->signal_note_removed().
  connect(mem_fun(*this, &NoteSelection::remove_note_internal));
  }


  NoteSelection& NoteSelection::operator=(const NoteSelection& sel) {
    notify_callbacks();
    m_data = sel.m_data;
    m_pat = sel.m_pat;
    if (m_pat)
      m_pat->signal_note_removed().
  connect(mem_fun(*this, &NoteSelection::remove_note_internal));
    return *this;
  }

  
  NoteSelection::Iterator NoteSelection::begin() const {
    return Iterator(m_data.begin());
  }


  NoteSelection::Iterator NoteSelection::end() const {
    return Iterator(m_data.end());
  }
  
  
  NoteSelection::Iterator 
  NoteSelection::find(const Pattern::NoteIterator& iter) const {
    return Iterator(m_data.find(iter));
  }
  
  
  NoteSelection::Iterator
  NoteSelection::add_note(const Pattern::NoteIterator& iter) {
    return m_data.insert(iter).first;
  }
  
  
  void NoteSelection::remove_note(const Pattern::NoteIterator& iter) {
    std::set<Pattern::NoteIterator>::iterator i = m_data.find(iter);
    if (i != m_data.end())
      m_data.erase(i);
  }


  void NoteSelection::clear() {
    m_data.clear();
  }


  void NoteSelection::printall() {
    // XXX This needs IMPLEMENTATION
    
    /*
    Iterator iter;
    cout<<"The selection contains "<<m_data.size()<<" elements:"<<endl;
    for (iter = begin(); iter != end(); ++iter)
      cout<<"  "<<iter->get_step()<<", "<<int(iter->get_key())<<endl;
    */
  }

  
  void NoteSelection::remove_note_internal(const Event& note) {
    const Event* ptr = &note;
    std::set<Pattern::NoteIterator>::iterator iter;
    for (iter = m_data.begin(); iter != m_data.end(); ++iter) {
      if (&**iter == ptr) {
	m_data.erase(iter);
	break;
      }
    }
  }

  
  const Pattern* NoteSelection::get_pattern() const {
    return m_pat;
  }


}
