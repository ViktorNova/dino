#include <iostream>
#include "deleter.hpp"


Deleter<MIDIEvent> g_event_deleter;
Deleter<TempoMap::TempoChange> g_tempochange_deleter;
