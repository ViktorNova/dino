#ifndef DELETER_HPP
#define DELETER_HPP

#include <typeinfo>
#include <cxxabi.h>

#include "debug.hpp"
#include "pattern.hpp"
#include "ringbuffer.hpp"
#include "tempomap.hpp"


//extern Ringbuffer<MIDIEvent*> g_notes_not_used;
//extern Ringbuffer<MIDIEvent*> g_notes_to_delete;

//bool queue_deletion(MIDIEvent* event);
//void do_delete();

template <class T>
class Deleter {
public:
  
  Deleter() : m_objects_not_used(1000), m_objects_to_delete(1000) {
    int status;
    dbg1<<"Initialising deallocator for "
	<<abi::__cxa_demangle(typeid(T).name(), 0, 0, &status)<<endl;
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
  
};


extern Deleter<MIDIEvent> g_event_deleter;
extern Deleter< CDTree<TempoMap::TempoChange*> > g_tempochange_deleter;


#endif
