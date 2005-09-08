#ifndef DELETER_HPP
#define DELETER_HPP

#include "pattern.hpp"
#include "ringbuffer.hpp"


extern Ringbuffer<MIDIEvent*> g_notes_not_used;
extern Ringbuffer<MIDIEvent*> g_notes_to_delete;

bool queue_deletion(MIDIEvent* event);
void do_delete();

template <class T>
class Deleter {
public:
  
  bool queue_deletion(T* pointer) {
    cerr<<"Queuing deletion of "<<pointer<<endl;
    return m_objects_not_used.push(pointer);
  }
  
  void do_delete() {
    T* pointer;
    while (m_objects_to_delete.pop(pointer)) {
      cerr<<"Deleting "<<pointer<<endl;
      delete pointer;
    }
  }
  
private:
  
  Ringbuffer<T*> m_objects_not_used;
  Ringbuffer<T*> m_objects_to_delete;
  
};


#endif
