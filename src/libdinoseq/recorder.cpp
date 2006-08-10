#include <glibmm.h>

#include "controller_numbers.hpp"
#include "debug.hpp"
#include "pattern.hpp"
#include "recorder.hpp"
#include "track.hpp"


using namespace Glib;
using namespace sigc;
using namespace std;


namespace Dino {


  Recorder::Recorder() 
    : m_track(0),
      m_track_set(false),
      m_to_audio(1024),
      m_from_audio(1024) {
    
  }
    

  void Recorder::set_track(int id) {
    Command c = { CommandTypeSetTrack, id };
    m_to_audio.push(c);
  }
  
  
  int Recorder::get_track() {
    return m_track;
  }


  void Recorder::run_editing_thread() {
    Event e;
    while (m_from_audio.pop(e)) {
      switch (e.type) {
      case EventTypeTrackSet:
        m_track = e.track_set.track;
        signal_track_set(m_track);
        break;
      }
    }
  }
    
  
  void Recorder::run_audio_thread() {
    Command c;
    while (m_to_audio.pop(c)) {
      switch (c.type) {
      case CommandTypeSetTrack:
        m_track_set = (c.set_track.track != 0);
        Event e = { EventTypeTrackSet, c.set_track.track };
        m_from_audio.push(e);
        break;
      }
    }
  }

  
}

