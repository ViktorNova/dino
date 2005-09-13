#include <iostream>
#include "deleter.hpp"


namespace Dino {


  Deleter<Dino::MIDIEvent> g_event_deleter;
  Deleter< CDTree<TempoMap::TempoChange*> > g_tempochange_deleter;


}
