#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include <string>

#include <asoundlib.h>
#include <glibmm/thread.h>


using namespace std;
using namespace Glib;


class JackClient;
class Song;


class Sequencer {
public:
  
  /** This struct contains information about a writable MIDI port. */
  struct InstrumentInfo {
    string name;
    int client;
    int port;
  };
  
  
  Sequencer(const string& client_name, Song& song);
  ~Sequencer();
  
  // transport functions
  void play();
  void stop();
  void go_to_beat(double beat);
  
  bool is_valid() const;
  int get_alsa_id() const;
  InstrumentInfo get_first_instrument();
  InstrumentInfo get_next_instrument();
  void set_instrument(int track, int client, int port);
  void reset_ports();
  
private:
  
  bool init_jack(const string& client_name);
  bool init_alsa(const string& client_name);
  
  void track_added(int track);
  void track_removed(int track);

  void sequencing_loop();
  void record_midi();
  void play_midi();
  void schedule_note(int beat, int tick, int port, int channel, 
		     int value, int velocity, int length);
  
  string m_client_name;
  /** No one is allowed to read or write anything in this variable without
      locking m_song.get_big_mutex() - the exception is the list of tempo 
      changes, which always must be readable for the JACK timebase callback. */
  Song& m_song;
  /** This is @c true if JACK and ALSA has been initialised succesfully. */
  bool m_valid;
  
  snd_seq_t* m_alsa_client;
  int m_alsa_queue;
  Thread* m_seq_thread;
  JackClient* m_jack_client;
};


#endif
