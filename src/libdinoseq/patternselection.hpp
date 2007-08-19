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

#ifndef PATTERNSELECTION_HPP
#define PATTERNSELECTION_HPP

#include <iterator>
#include <set>

#include <sigc++/trackable.h>

#include "pattern.hpp"


namespace Dino {
  
  
  /** This class represents a subset of the set of all notes in a Pattern.
      It is used by the GUI to store selections.
  */
  class PatternSelection : public sigc::trackable {
  public:
    
    /** This iterator provides a way to iterate over all notes in this
  selection. It can be automatically casted to a Pattern::NoteIterator,
  so it can be used with all functions that expect a Pattern::NoteIterator
  object as a parameter.
    */
    class Iterator : public std::iterator<std::forward_iterator_tag, Note> {
    public:
      
      /** Create an invalid iterator. */
      Iterator() { }
      
      /** Dereference the iterator to get a constant Note reference. */
      Note& operator*();
      /** Dereference the iterator to get a constant Note pointer. */
      Note* operator->();
      /** Compare two iterators for equality. */
      bool operator==(const Iterator& iter) const;
      /** Compare two iterators for inequality. */
      bool operator!=(const Iterator& iter) const;
      /** A cast operator that casts this iterator to a Pattern::NoteIterator
    that can be used with the mutator functions in the Pattern class. */
      operator Pattern::NoteIterator () const;
      
      /** Advance the iterator one step. */
      Iterator& operator++();
      Iterator operator++(int);
      
    protected:
      
      /** PatternSelection is a friend since we only want that class to be
    able to create new valid iterators. */
      friend class PatternSelection;
      
      /** This constructor is used by PatternSelection to create valid 
    iterators. */
      Iterator(const std::set<Pattern::NoteIterator>::iterator& iterator);
      
      /** The actual iterator over the selection set. */
      std::set<Pattern::NoteIterator>::iterator m_iter;
    };
    
    
    /** Creates a new selection. If @c pat is 0 the selection is invalid
	and should not be used for anything. */
    PatternSelection(Pattern* pat = 0);
    /** Copy a selection. */
    PatternSelection(const PatternSelection& sel);
    /** Copy a selection. */
    PatternSelection& operator=(const PatternSelection& sel);
    
    /** Returns an iterator to the first note in the selection. */
    Iterator begin() const;
    /** Returns an iterator to the end of the selection that can be used
  to compare other iterators to. */
    Iterator end() const;
    /** Returns an iterator to the given Pattern::NoteIterator, or end() if
  that note is not in this selection. */
    Iterator find(const Pattern::NoteIterator& iter) const;
    
    /** Returns the Pattern that this object is a selection over. */
    Pattern* get_pattern();
    const Pattern* get_pattern() const;
    /** Adds a note to the selection. */
    Iterator add_note(const Pattern::NoteIterator& iter);
    /** Removes a note from the selection. */
    void remove_note(const Pattern::NoteIterator& iter);
    /** Clears the entire selection by removing all notes. */
    void clear();
    
    /** For debugging only. Should be XXX removed sometime. */
    void printall();
    
  protected:
    
    /** Used to remove notes from the selection automatically when they are
	removed from the pattern. */
    void remove_note_internal(const Note& note);
    
    /** The actual set of selected notes. */
    std::set<Pattern::NoteIterator> m_data;
    
    /** The pattern that this selection is a selection over. */
    Pattern* m_pat;
    
  };

}


#endif
