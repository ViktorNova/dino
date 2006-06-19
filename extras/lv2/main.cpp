#include <iostream>
#include <vector>
#include <unistd.h>

#include <jack/jack.h>
#include <lo/lo.h>

#include "lv2host.hpp"


using namespace std;


vector<jack_port_t*> jack_ports;
jack_client_t* jack_client;


/** The JACK process callback */
int process(jack_nframes_t nframes, void* arg) {
  LV2Host* host = static_cast<LV2Host*>(arg);
  for (size_t i = 0; i < host->get_ports().size(); ++i) {
    if (jack_ports[i])
      host->get_ports()[i].buffer = jack_port_get_buffer(jack_ports[i], nframes);
  }
  host->run(nframes);
}


/** DSSI-style OSC method handler for /control <int> <float> */
int control_callback(const char* path, const char* types, 
                     lo_arg** argv, int argc, lo_message msg, void* user_data) {
  LV2Host* host = static_cast<LV2Host*>(user_data);
  int port = argv[0]->i;
  float value = argv[0]->f;
  if (port < host->get_ports().size())
    // XXX not threadsafe
    *static_cast<float*>(host->get_ports()[port].buffer) = value;
}


int main(int argc, char** argv) {
  
  if (argc < 2) {
    cerr<<"Load what plugin?"<<endl;
    return 1;
  }
  
  // load plugin
  LV2Host lv2h(argv[1], 48000);

  if (lv2h.is_valid()) {
    
    // initialise JACK client
    jack_client = jack_client_new("LV2Host");
    for (size_t i = 0; i < lv2h.get_ports().size(); ++i) {
      jack_port_t* port = 0;
      LV2Port& lv2port = lv2h.get_ports()[i];
      if (lv2port.port_class == SLV2_AUDIO_RATE_INPUT) {
        port = jack_port_register(jack_client, lv2port.symbol.c_str(),
                                  JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
      }
      else if (lv2port.port_class == SLV2_AUDIO_RATE_OUTPUT) {
        port = jack_port_register(jack_client, lv2port.symbol.c_str(),
                                  JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
      }
      else if (lv2port.port_class == SLV2_CONTROL_RATE_INPUT)
        lv2port.buffer = new float(lv2port.default_value);
      else if (lv2port.port_class == SLV2_CONTROL_RATE_OUTPUT)
        lv2port.buffer = new float;
      jack_ports.push_back(port);
    }
    jack_set_process_callback(jack_client, &process, &lv2h);
    jack_activate(jack_client);
    
    // initialise OSC server
    lo_server_thread osc_server = lo_server_thread_new(0, 0);
    lo_server_thread_add_method(osc_server, "/control", "if", 
                                &control_callback, &lv2h);
    lo_server_thread_start(osc_server);
    
    // wait until we are killed
    while (true)
      usleep(10000);
    
    lo_server_thread_stop(osc_server);
    jack_client_close(jack_client);
  }
  
  else {
    cerr<<"Could not find the plugin with URI <"<<argv[1]<<">"<<endl;
    return 1;
  }
  
  return 0;
}
