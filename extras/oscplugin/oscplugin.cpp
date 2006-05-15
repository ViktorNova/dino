#include <iostream>

#include <debug.hpp>
#include <plugininterface.hpp>
#include <ringbuffer.hpp>
#include <sequencer.hpp>

#include <lo/lo.h>

#include <glibmm/dispatcher.h>


using namespace std;
using namespace Dino;
using namespace Glib;
using namespace sigc;


// variables and functions local to the plugin
namespace {
  
  // the OSC server thread
  lo_server_thread serverthread;
  
  // dispatchers are needed since the OSC server runs in another thread
  Dispatcher dp_stop;
  Dispatcher dp_play;
  Dispatcher dp_relocate;
  
  // this is needed to pass the parameters for dp_relocate since dispatchers
  // don't take parameters
  Ringbuffer<float> rbuf(256);
  
  
  // this gets called in the OSC server thread when /dino/stop is received
  int stop_handler(const char* path, const char* types, lo_arg** argv, 
		   int argc, lo_message msg, void* user_data) {
    dp_stop();
    return 0;
  }

  // this gets called in the OSC server thread when /dino/play is received
  int play_handler(const char* path, const char* types, lo_arg** argv, 
		   int argc, lo_message msg, void* user_data) {
    dp_play();
    return 0;
  }

  // this gets called in the OSC server thread when /dino/relocate is received
  int relocate_handler(const char* path, const char* types, lo_arg** argv, 
		       int argc, lo_message msg, void* user_data) {
    rbuf.push(argv[0]->f);
    dp_relocate();
    return 0;
  }
  
}


// required plugin interface
extern "C" {
  
  string dino_get_name() {
    return "OSC plugin";
  }
  
  void dino_load_plugin(PluginInterface& plif) {
    
    // sigc connections
    dp_stop.connect(mem_fun(plif.get_sequencer(), &Sequencer::stop));
    dp_play.connect(mem_fun(plif.get_sequencer(), &Sequencer::play));
    float (Ringbuffer<float>::*vpop)() = &Ringbuffer<float>::pop;
    dp_relocate.connect(compose(mem_fun(plif.get_sequencer(), 
					&Sequencer::go_to_beat), 
				mem_fun(rbuf, vpop)));
    
    // add OSC method handlers and start the server
    serverthread = lo_server_thread_new(0, 0);
    cout<<"Dino OSC server is listening on port "
	<<lo_server_thread_get_port(serverthread)<<endl;
    lo_server_thread_add_method(serverthread, "/dino/play", "", 
				&play_handler, 0);
    lo_server_thread_add_method(serverthread, "/dino/stop", "", 
				&stop_handler, 0);
    lo_server_thread_add_method(serverthread, "/dino/relocate", "f", 
				&relocate_handler, 0);
    rbuf.clear();
    lo_server_thread_start(serverthread);
  }
  
  void dino_unload_plugin() {
    lo_server_thread_free(serverthread);
  }
  
}


