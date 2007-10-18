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

#include "note.hpp"
#include "notecollection.hpp"
#include "noteselection.hpp"


using namespace std;


namespace Dino {
  
  
  NoteCollection::NoteDescription::NoteDescription(unsigned int _start, 
						   unsigned int _length,
						   unsigned char _key, 
						   unsigned char _velocity)
    : start(_start),
      length(_length),
      key(_key),
      velocity(_velocity) {
    
  }


  NoteCollection::NoteCollection() {

  }


  NoteCollection::NoteCollection(const NoteSelection& selection) {
    NoteSelection::Iterator iter;
    for (iter = selection.begin(); iter != selection.end(); ++iter) {
      m_data.push_back(NoteDescription(iter->get_step(), iter->get_length(),
                                       iter->get_key(), iter->get_velocity()));
    }
  }
  
  
  NoteCollection::Iterator NoteCollection::begin() {
    return m_data.begin();
  }
  
  
  NoteCollection::ConstIterator NoteCollection::begin() const {
    return m_data.begin();
  }
  
  
  NoteCollection::Iterator NoteCollection::end() {
    return m_data.end();
  }
   

  NoteCollection::ConstIterator NoteCollection::end() const {
    return m_data.end();
  }


  void NoteCollection::printall() const {
    cout<<"NoteCollection:"<<endl;
    for (unsigned i = 0; i < m_data.size(); ++i) {
      cout<<"("<<m_data[i].start<<", "<<m_data[i].length<<", "
          <<m_data[i].key<<", "<<m_data[i].velocity<<")"<<endl;
    }
    
  }


  bool NoteCollection::add_note(unsigned int start, unsigned int length, 
				unsigned char key, unsigned char velocity) {
    for (unsigned i = 0; i < m_data.size(); ++i) {
      if (m_data[i].key == key &&
	  (m_data[i].start < start + length && 
	   m_data[i].start + m_data[i].length > start))
	return false;
    }
    m_data.push_back(NoteDescription(start, length, key, velocity));
    return true;
  }


  void NoteCollection::clear() {
    m_data.clear();
  }
  
}

