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

  
  /** This class is used to delete objects where the sequencer thread has
      to confirm that it isn't using them before we can delete them. It's
      a template class so we can create deleters for any class, but at the
      moment all objects that we delete in this way inherit from Deletable,
      so only one template instantiation is needed. */
  template <class T>
  class Deleter {
  public:
    
    /** This constructor initialises the two ringbuffers and sets up a cleanup
	function to delete objects every 100 milliseconds. */
    Deleter() : m_objects_not_used(1000), m_objects_to_delete(1000) {
      dbg1<<"Initialising deallocator for "<<demangle(typeid(T).name())<<endl;
      m_connection = signal_timeout().
	connect(bind_return(mem_fun(*this, &Deleter<T>::do_delete), true), 100);
    }
    
    ~Deleter() {
      dbg1<<"Destroying deallocator for "<<demangle(typeid(T).name())<<endl;
      m_connection.disconnect();
    }
    
    /** Queue an object for deletion. */
    bool queue_deletion(T* pointer) {
      //cerr<<"Queuing deletion of "<<pointer<<endl;
      return m_objects_not_used.push(pointer);
    }
    
    /** This is the function that does the actual deallocation of the objects.
	It is run automatically a couple of times every second. */
    void do_delete() {
      T* pointer;
      while (m_objects_to_delete.pop(pointer)) {
	//cerr<<"Deleting "<<pointer<<endl;
	delete pointer;
      }
    }
    
    /** This is the function that is called in the sequencer thread to confirm
	that the objects queued to be deleted are not in use. It simply pops
	pointers from one queue and pushes them onto another. */
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
