#include "commandproxy.hpp"


namespace Dino {


  CommandProxy::CommandProxy(Song& song, Sequencer& seq)
    : m_song(song),
      m_seq(seq) {

  }
    
  
  bool CommandProxy::undo() {
    return false;
  }
    
  
  bool CommandProxy::can_undo() const {
    return false;
  }
  
  
  std::string CommandProxy::get_next_undo_name() const {
    return "";
  }
  
  
}
