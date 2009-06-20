/*****************************************************************************
    libdinoseq - a library for MIDI sequencing
    Copyright (C) 2009  Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef ATOMICINT_HPP
#define ATOMICINT_HPP

#include <glib.h>

namespace Dino {


  /** An atomic integer class. It has two functions,
      get() and set() that reads and writes the integer using atomic 
      operations. They use the atomic operations defined in Glib, so they are
      atomic and lock-free as long as Glib's operations are (should work on 
      all common hardware platforms). They also act as memory barriers. This 
      makes them well suited as building blocks for lock-free data structures.
  */
  class AtomicInt {
  public:
    
    /** This is the underlying integer type for the AtomicInt class. */
    typedef gint Type;
    
    
    /** Initialise the atomic integer to the value of @c t. This operation is
	@b not atomic. */
    AtomicInt(Type value);
    
    /** Return the value of the atomic integer as a normal pinteger. This is
	an atomic and lock-free operation, and it's also a memory barrier. */
    Type get() const;
    
    /** Set the value of the atomic integer. This is an atomic and lock-free
	operation and also a memory barrier. */
    void set(Type new_value);
    
    /** Increase the atomic integer by 1. This is an atomic and lock-free 
	operation and also a memory barrier. */
    void increase();
    
  private:
    
    Type m_data;
    
  };
  

}


#endif
