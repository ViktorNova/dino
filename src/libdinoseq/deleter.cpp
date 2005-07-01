#include <iostream>
#include "deleter.hpp"


Ringbuffer<Pattern::NoteEvent*> g_notes_not_used(1000);
Ringbuffer<Pattern::NoteEvent*> g_notes_to_delete(1000);


bool queue_deletion(Pattern::NoteEvent* event) {
  cerr<<"Queuing deletion of "<<event<<endl;
  return g_notes_not_used.push(event);
}


void do_delete() {
  Pattern::NoteEvent* event;
  while (g_notes_to_delete.pop(event)) {
    cerr<<"Deleting "<<event<<endl;
    delete event;
  }
}

