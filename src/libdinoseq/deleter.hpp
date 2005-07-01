#ifndef DELETER_HPP
#define DELETER_HPP

#include "pattern.hpp"
#include "ringbuffer.hpp"


extern Ringbuffer<MIDIEvent*> g_notes_not_used;
extern Ringbuffer<MIDIEvent*> g_notes_to_delete;

bool queue_deletion(MIDIEvent* event);
void do_delete();


#endif
