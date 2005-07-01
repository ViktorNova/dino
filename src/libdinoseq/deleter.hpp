#ifndef DELETER_HPP
#define DELETER_HPP

#include "pattern.hpp"
#include "ringbuffer.hpp"


extern Ringbuffer<Pattern::NoteEvent*> g_notes_not_used;
extern Ringbuffer<Pattern::NoteEvent*> g_notes_to_delete;

bool queue_deletion(Pattern::NoteEvent* event);
void do_delete();


#endif
