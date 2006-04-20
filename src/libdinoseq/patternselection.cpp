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

#include <iostream>
#include <typeinfo>

#include "patternselection.hpp"


using namespace std;


namespace Dino {


  const Note* PatternSelection::Iterator::operator*() const {
    return **m_iter;
  }
  
  
  const Note* PatternSelection::Iterator::operator->() const {
    return **m_iter;
  }

  
  bool PatternSelection::Iterator::operator==(const Iterator& iter) const {
    return (m_iter == iter.m_iter);
  }
  
  
  bool PatternSelection::Iterator::operator!=(const Iterator& iter) const {
    return (m_iter != iter.m_iter);
  }
  
  
  PatternSelection::Iterator::operator Pattern::NoteIterator() const {
    return *m_iter;
  }
    
  
  PatternSelection::Iterator& PatternSelection::Iterator::operator++() {
    ++m_iter;
    return *this;
  }
  
    
  PatternSelection::Iterator::
  Iterator(const std::set<Pattern::NoteIterator>::iterator& iterator) 
    : m_iter(iterator) {

  }
  
  
  PatternSelection::PatternSelection(Pattern* pat) {
    if (pat) {
      pat->signal_note_removed.
	connect(mem_fun(*this, &PatternSelection::remove_note_internal));
    }
  }

  
  PatternSelection::Iterator PatternSelection::begin() const {
    return Iterator(m_data.begin());
  }


  PatternSelection::Iterator PatternSelection::end() const {
    return Iterator(m_data.end());
  }
  
  
  PatternSelection::Iterator 
  PatternSelection::find(const Pattern::NoteIterator& iter) const {
    return Iterator(m_data.find(iter));
  }
  
  
  PatternSelection::Iterator
  PatternSelection::add_note(const Pattern::NoteIterator& iter) {
    return m_data.insert(iter).first;
  }
  
  
  void PatternSelection::remove_note(const Pattern::NoteIterator& iter) {
    std::set<Pattern::NoteIterator>::iterator i = m_data.find(iter);
    if (i != m_data.end())
      m_data.erase(i);
  }


  void PatternSelection::printall() {
    Iterator iter;
    cout<<"The selection contains "<<m_data.size()<<" elements:"<<endl;
    for (iter = begin(); iter != end(); ++iter)
      cout<<"  "<<(*iter)->get_step()<<", "<<int((*iter)->get_key())<<endl;
  }

  
  void PatternSelection::remove_note_internal(const Note& note) {
    const Note* ptr = &note;
    std::set<Pattern::NoteIterator>::iterator iter;
    for (iter = m_data.begin(); iter != m_data.end(); ++iter) {
      if (**iter == ptr) {
	m_data.erase(iter);
	break;
      }
    }
  }

}
