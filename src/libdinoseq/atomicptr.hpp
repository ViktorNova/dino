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

#ifndef ATOMICPTR_HPP
#define ATOMICPTR_HPP

#include <glib.h>

namespace Dino {


  /** An atomic wrapper class template for pointers. It has two functions,
      get() and set() that reads and writes the pointer using atomic 
      operations. They use the atomic operations defined in Glib, so they are
      atomic and lock-free as long as Glib's operations are (should work on 
      all common hardware platforms). They also act as memory barriers. This 
      makes them well suited as building blocks for lock-free data structures.
  */
  template <typename T>
  class AtomicPtr {
  public:
    
    /** Initialise the atomic pointer to the value of @c t. This operation is
	@b not atomic. */
    AtomicPtr(T* t) : m_pointer(t) { }
    
    /** Return the value of the atomic pointer as a normal pointer. This is
	an atomic and lock-free operation, and it's also a memory barrier. */
    T* get() { return static_cast<T*>(g_atomic_pointer_get(&m_pointer)); }
    
    /** Set the value of the atomic pointer. This is an atomic and lock-free
	operation and also a memory barrier. */
    void set(T* new_value) { g_atomic_pointer_set(&m_pointer, new_value); }
    
  private:
    
    gpointer m_pointer;
    
  };
  

}


#endif
