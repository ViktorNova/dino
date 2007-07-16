#include <string>


namespace Dino {
  
  
  class Sequencer;
  class Song;


  class CommandProxy {
  public:
    
    CommandProxy(Song& song, Sequencer& seq);
    
    bool undo();
    
    bool can_undo() const;
    std::string get_next_undo_name() const;
    
  protected:
    
    Sequencer& m_seq;
    Song& m_song;
    
  };
  
  
}
