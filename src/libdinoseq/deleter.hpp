#ifndef DELETER_HPP
#define DELETER_HPP

#include <glibmm.h>
#include <sigc++/sigc++.h>
#include "debug.hpp"
#include "ringbuffer.hpp"
#include "deletable.hpp"

using namespace Glib;
using namespace sigc;


namespace Dino {


  template <class T>
  class Deleter {
  public:
  
    Deleter() : m_objects_not_used(1000), m_objects_to_delete(1000) {
      dbg1<<"Initialising deallocator for "<<demangle(typeid(T).name())<<endl;
      m_connection = signal_timeout().
	connect(bind_return(mem_fun(*this, &Deleter<T>::do_delete), true), 100);
    }
    
    ~Deleter() {
      dbg1<<"Destroying deallocator for "<<demangle(typeid(T).name())<<endl;
      m_connection.disconnect();
    }
    
    bool queue_deletion(T* pointer) {
      //cerr<<"Queuing deletion of "<<pointer<<endl;
      return m_objects_not_used.push(pointer);
    }
  
    void do_delete() {
      T* pointer;
      while (m_objects_to_delete.pop(pointer)) {
	//cerr<<"Deleting "<<pointer<<endl;
	delete pointer;
      }
    }
  
    void confirm(unsigned int n = 100) {
      T* pointer;
      for (unsigned int i = 0; i < n && m_objects_not_used.pop(pointer); ++i) 
	m_objects_to_delete.push(pointer);
    }
  
  private:
  
    Ringbuffer<T*> m_objects_not_used;
    Ringbuffer<T*> m_objects_to_delete;
    connection m_connection;
  };


  extern Deleter<Deletable> g_deletable_deleter;

}


#endif
