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
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#ifndef NOTECOLLECTION_HPP
#define NOTECOLLECTION_HPP

#include <vector>


namespace Dino {
  
  class PatternSelection;
  
  
  /** This class is used to describe collections of notes without containing
      any references to actual Note or Pattern objects. It can be used as
      a sort of "clipboard format" for copying and pasting notes without
      depending on the pattern that they were copied from. */
  class NoteCollection {
  public:
    
    /** A description of a single note. */
    struct NoteDescription {
      NoteDescription(unsigned int start, unsigned int length,
          unsigned char key, unsigned char velocity);
      unsigned int start;
      unsigned int length;
      unsigned char key;
      unsigned char velocity;
    };
    
    /** An iterator type that iterates over the NoteDescription objects in
  this collection. */
    typedef std::vector<NoteDescription>::iterator Iterator;
    
    /** A const iterator type that iterates over the NoteDescription objects in
  this collection. */
    typedef std::vector<NoteDescription>::const_iterator ConstIterator;
    
    /** Create a new empty NoteCollection. */
    NoteCollection();
    
    /** Create a new NoteCollection from the notes in a PatternSelection. */
    NoteCollection(const PatternSelection& selection);
    
    /** Return an iterator that points to the first NoteDescription object. */
    Iterator begin();
    ConstIterator begin() const;
    Iterator end();
    ConstIterator end() const;
    
    /** Debug output. */
    void printall() const;
    
  protected:
    
    std::vector<NoteDescription> m_data;
    
  };


}


#endif
