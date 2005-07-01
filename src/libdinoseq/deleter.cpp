#include <iostream>
#include "deleter.hpp"


Ringbuffer<MIDIEvent*> g_notes_not_used(1000);
Ringbuffer<MIDIEvent*> g_notes_to_delete(1000);


bool queue_deletion(MIDIEvent* event) {
  cerr<<"Queuing deletion of "<<event<<endl;
  return g_notes_not_used.push(event);
}


void do_delete() {
  MIDIEvent* event;
  while (g_notes_to_delete.pop(event)) {
    cerr<<"Deleting "<<event<<endl;
    delete event;
  }
}

