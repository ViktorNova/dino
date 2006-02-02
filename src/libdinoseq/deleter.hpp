#ifndef DELETER_HPP
#define DELETER_HPP

#include <glibmm.h>

#include "ringbuffer.hpp"


namespace Dino {

  
  class DeletableBase {
  public:
    virtual ~DeletableBase() { }
  };
  
  
  template <class T> class Deletable : public DeletableBase {
  public:
    Deletable(T* pointer) : m_pointer(pointer) { }
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
  
    Ringbuffer<DeletableBase*> m_objects_not_used;
    Ringbuffer<DeletableBase*> m_objects_to_delete;
    sigc::connection m_connection;
    
  };
  

}


#endif
