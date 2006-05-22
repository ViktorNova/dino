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

#ifndef DELETER_HPP
#define DELETER_HPP

#include <glibmm.h>

#include "ringbuffer.hpp"


namespace Dino {

  
  /** Need a base class for the template class so we can use it 
      polymorphically. */
  class DeletableBase {
  public:
    /** Need a virtual destructor so the destructors of the subclasses get
	called. */
    virtual ~DeletableBase() { }
  };
  
  
  /** A template class that wraps a pointer of any type so it can be
      deleted by the Deleter. */
  template <class T> class Deletable : public DeletableBase {
  public:
    /** The constructor just stores a copy of the pointer. */
    Deletable(T* pointer) : m_pointer(pointer) { }
    /** The destructor actually deletes the wrapped object. */
    ~Deletable() { delete m_pointer; }
  private:
    T* m_pointer;
  };
  
  
  class Deleter {
  public:
  
    /** This constructor initialises the two ringbuffers and sets up a cleanup
	function to delete objects every 100 milliseconds. */
    Deleter() : m_objects_not_used(1000), m_objects_to_delete(1000) {
      using namespace sigc;
      dbg1<<"Initialising threadsafe deallocator"<<endl;
      m_connection = Glib::signal_timeout().
	connect(bind_return(mem_fun(*this, &Deleter::do_delete), true), 100);
    }
    
    ~Deleter() {
      dbg1<<"Destroying threadsafe deallocator"<<endl;
      m_connection.disconnect();
    }
    
    /** Returns the Deleter singleton object. */
    static Deleter& get_instance() {
      static Deleter d;
      return d;
    }
    
    /** Queue an object for deletion. */
    template <class T> static bool queue(T* pointer) {
      return get_instance().m_objects_not_used.push(new Deletable<T>(pointer));
    }
    
    /** This is the function that does the actual deallocation of the objects.
	It is run automatically a couple of times every second. */
    void do_delete() {
      DeletableBase* pointer;
      while (m_objects_to_delete.pop(pointer)) {
	delete pointer;
      }
    }
    
    /** This is the function that is called in the sequencer thread to confirm
	that the objects queued to be deleted are not in use. It simply pops
	pointers from one queue and pushes them onto another. */
    void confirm(unsigned int n = 100) {
      DeletableBase* pointer;
      for (unsigned int i = 0; i < n && m_objects_not_used.pop(pointer); ++i) 
	m_objects_to_delete.push(pointer);
    }
  
  private:
  
    /** A ringbuffer containing pointers to the objects that are no longer 
	used by the deleting thread but still not safe to delete. */
    Ringbuffer<DeletableBase*> m_objects_not_used;
    
    /** A ringbuffer containing pointers to the objects that are now safe
	to delete. */
    Ringbuffer<DeletableBase*> m_objects_to_delete;
    
    sigc::connection m_connection;
    
  };
  

}


#endif
