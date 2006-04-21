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

#ifndef PATTERNSELECTION_HPP
#define PATTERNSELECTION_HPP

#include <set>

#include <sigc++/trackable.h>

#include "pattern.hpp"


namespace Dino {
  

  class PatternSelection : public sigc::trackable {
  public:
    
    class Iterator {
    public:
      
      Iterator() { }
      
      const Note* operator*() const;
      const Note* operator->() const;
      bool operator==(const Iterator& iter) const;
      bool operator!=(const Iterator& iter) const;
      operator Pattern::NoteIterator () const;
      
      Iterator& operator++();
      
    protected:
      
      friend class PatternSelection;
      
      Iterator(const std::set<Pattern::NoteIterator>::iterator& iterator);
      
      std::set<Pattern::NoteIterator>::iterator m_iter;
    };
    
    PatternSelection(Pattern* pat = 0);
    PatternSelection(const PatternSelection& sel);
    PatternSelection& operator=(const PatternSelection& sel);
    
    Iterator begin() const;
    Iterator end() const;
    Iterator find(const Pattern::NoteIterator& iter) const;
    
    Iterator add_note(const Pattern::NoteIterator& iter);
    void remove_note(const Pattern::NoteIterator& iter);
    void clear();
    
    void printall();
    
  protected:

    void remove_note_internal(const Note& note);
    
    std::set<Pattern::NoteIterator> m_data;
    
    Pattern* m_pat;
    
  };

}


#endif
