#ifndef DINOOBJECT_HPP
#define DINOOBJECT_HPP

#include "object.hpp"


namespace Dino {
  class CommandProxy;
}


namespace DBus {
  class Argument;
}


class DinoObject : public DBus::Object {
public:
  
  DinoObject(Dino::CommandProxy& proxy);

protected:
  
  bool play(int argc, DBus::Argument* argv);
  bool stop(int argc, DBus::Argument* argv);
  bool go_to_beat(int argc, DBus::Argument* argv);
  
  bool set_song_title(int argc, DBus::Argument* argv);
  bool set_song_author(int argc, DBus::Argument* argv);
  bool set_song_info(int argc, DBus::Argument* argv);
  bool set_song_length(int argc, DBus::Argument* argv);
  bool set_loop_start(int argc, DBus::Argument* argv);
  bool set_loop_end(int argc, DBus::Argument* argv);
  bool add_track(int argc, DBus::Argument* argv);
  bool remove_track(int argc, DBus::Argument* argv);
  bool add_tempo_change(int argc, DBus::Argument* argv);
  bool remove_tempo_change(int argc, DBus::Argument* argv);

  Dino::CommandProxy& m_proxy;

};


#endif
