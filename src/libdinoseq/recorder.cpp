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
    : m_track(0) {
    
  }
    

  void Recorder::set_track(int id) {
    m_track = id;
  }
  
  
  int Recorder::get_track() {
    return m_track;
  }

  
}

