#include <iostream>
#include <vector>
#include <unistd.h>

#include <jack/jack.h>
#include <jack/midiport.h>
#include <lo/lo.h>
#include <lo/lo_lowlevel.h>

#include "lv2host.hpp"
#include "lv2-miditype.h"


using namespace std;


vector<jack_port_t*> jack_ports;
jack_client_t* jack_client;


/** Translate from an LV2 MIDI buffer to a JACK MIDI buffer. */
void lv2midi2jackmidi(LV2Port& port, jack_port_t* jack_port, 
                      jack_nframes_t nframes) {
  jack_nframes_t timestamp;
  size_t data_size;
  void* output_buf = jack_port_get_buffer(jack_port, nframes);
  LV2_MIDI* input_buf = static_cast<LV2_MIDI*>(port.buffer);
  
  jack_midi_clear_buffer(output_buf, nframes);
  
  // iterate over all MIDI events and write them to the JACK port
  unsigned char* data = input_buf->data;
  
  for (size_t i = 0; i < input_buf->event_count; ++i) {
    
    // retrieve LV2 MIDI event
    timestamp = static_cast<jack_nframes_t>(*reinterpret_cast<double*>(data));
    data += sizeof(double);
    data_size = *reinterpret_cast<size_t*>(data);
    data += sizeof(size_t);
    
    // write JACK MIDI event
    jack_midi_event_write(output_buf, timestamp, 
                          reinterpret_cast<jack_midi_data_t*>(data),
                          data_size, nframes);
    data += data_size;
  }
}


/** Translate from a JACK MIDI buffer to an LV2 MIDI buffer. */
void jackmidi2lv2midi(jack_port_t* jack_port, LV2Port& port,
                      jack_nframes_t nframes) {
  void* input_buf = jack_port_get_buffer(jack_port, nframes);
  jack_midi_event_t input_event;
  jack_nframes_t input_event_index = 0;
  jack_nframes_t input_event_count = 
    jack_midi_port_get_info(input_buf, nframes)->event_count;
  jack_nframes_t timestamp;
  LV2_MIDI* output_buf = static_cast<LV2_MIDI*>(port.buffer);
  output_buf->event_count = 0;
  
  // iterate over all incoming JACK MIDI events
  unsigned char* data = output_buf->data;
  for (unsigned int i = 0; i < input_event_count; ++i) {
    
    // retrieve JACK MIDI event
    jack_midi_event_get(&input_event, input_buf, i, nframes);
    if ((data - output_buf->data) + sizeof(double) + 
        sizeof(size_t) + input_event.size >= output_buf->capacity)
      break;
    
    // write LV2 MIDI event
    *reinterpret_cast<double*>(data) = input_event.time;
    data += sizeof(double);
    *reinterpret_cast<size_t*>(data) = input_event.size;
    data += sizeof(size_t);
    memcpy(data, input_event.buffer, input_event.size);
    
    // normalise note events if needed
    if ((input_event.size == 3) && ((data[0] & 0xF0) == 0x90) && (data[2] == 0))
      data[0] = 0x80 | (data[0] & 0x0F);
    
    data += input_event.size;
    ++output_buf->event_count;
  }

  output_buf->used_capacity = data - output_buf->data;
  
}


/** The JACK process callback */
int process(jack_nframes_t nframes, void* arg) {
  
  LV2Host* host = static_cast<LV2Host*>(arg);
  
  // iterate over all ports and copy data from JACK ports to audio and MIDI
  // ports in the plugin
  for (size_t i = 0; i < host->get_ports().size(); ++i) {
    
    // does this plugin port have an associated JACK port?
    if (jack_ports[i]) {

      LV2Port& port = host->get_ports()[i];
      
      // audio port, just copy the buffer pointer.
      if (!port.midi)
        port.buffer = jack_port_get_buffer(jack_ports[i], nframes);
      
      // MIDI input port, copy the events one by one
      else if (port.port_class == SLV2_CONTROL_RATE_INPUT)
        jackmidi2lv2midi(jack_ports[i], port, nframes);
      
    }
  }
  
  // run the plugin!
  host->run(nframes);
  
  // Copy events from MIDI output ports to JACK ports
  for (size_t i = 0; i < host->get_ports().size(); ++i) {
    if (jack_ports[i]) {
      LV2Port& port = host->get_ports()[i];
      if (port.midi && port.port_class == SLV2_CONTROL_RATE_OUTPUT)
        lv2midi2jackmidi(port, jack_ports[i], nframes);
    }
  }
  
  return 0;
}


/** DSSI-style OSC method handler for /control <int> <float> */
int control_callback(const char* path, const char* types, 
                     lo_arg** argv, int argc, lo_message msg, void* user_data) {
  LV2Host* host = static_cast<LV2Host*>(user_data);
  int port = argv[0]->i;
  float value = argv[1]->f;
  cerr<<"in dino-lv2host: "<<port<<" "<<value<<endl;
  if (port < host->get_ports().size())
    // XXX not threadsafe! but this is not a serious host anyway.
    *static_cast<float*>(host->get_ports()[port].buffer) = value;
  
  return 0;
}


int main(int argc, char** argv) {
  
  if (argc < 2) {
    cerr<<"usage: "<<argv[0]<<" PLUGIN_URI"<<endl
        <<"example: "<<argv[0]
        <<" 'http://ll-plugins.nongnu.org/lv2/sineshaper/0.0.0'"<<endl;
    return 1;
  }
  
  // load plugin
  LV2Host lv2h(argv[1], 48000);

  if (lv2h.is_valid()) {
    
    
    cerr<<"MIDI map:"<<endl;
    for (unsigned i = 0; i < 127; ++i)
      cerr<<"  "<<i<<" -> "<<lv2h.get_midi_map()[i]<<endl;
    cerr<<endl;
    
    
    // initialise JACK client and plugin port buffers
    jack_client = jack_client_new("LV2Host");
    for (size_t i = 0; i < lv2h.get_ports().size(); ++i) {
      jack_port_t* port = 0;
      LV2Port& lv2port = lv2h.get_ports()[i];
      
      // add input port
      if (lv2port.port_class == SLV2_AUDIO_RATE_INPUT || 
          (lv2port.port_class == SLV2_CONTROL_RATE_INPUT && lv2port.midi)) {
        port = jack_port_register(jack_client, lv2port.symbol.c_str(),
                                  (lv2port.midi ? JACK_DEFAULT_MIDI_TYPE :
                                   JACK_DEFAULT_AUDIO_TYPE),
                                  JackPortIsInput, 0);
        if (lv2port.midi) {
          LV2_MIDI* mbuf = new LV2_MIDI;
          mbuf->capacity = 8192;
          mbuf->data = new unsigned char[8192];
          lv2port.buffer = mbuf;
        }
      }
      
      // add output port
      else if (lv2port.port_class == SLV2_AUDIO_RATE_OUTPUT || 
               (lv2port.port_class == SLV2_CONTROL_RATE_OUTPUT && lv2port.midi)){
        port = jack_port_register(jack_client, lv2port.symbol.c_str(),
                                  (lv2port.midi ? JACK_DEFAULT_MIDI_TYPE :
                                   JACK_DEFAULT_AUDIO_TYPE),
                                  JackPortIsOutput, 0);
        if (lv2port.midi) {
          LV2_MIDI* mbuf = new LV2_MIDI;
          mbuf->capacity = 8192;
          mbuf->data = new unsigned char[8192];
          lv2port.buffer = mbuf;
        }
      }
      
      // for control ports, just create buffers consisting of a single float
      else if (lv2port.port_class == SLV2_CONTROL_RATE_INPUT)
        lv2port.buffer = new float(lv2port.default_value);
      else if (lv2port.port_class == SLV2_CONTROL_RATE_OUTPUT)
        lv2port.buffer = new float;
      
      jack_ports.push_back(port);
    }
    jack_set_process_callback(jack_client, &process, &lv2h);
    lv2h.activate();
    jack_activate(jack_client);
    
    // initialise OSC server
    lo_server_thread osc_server = lo_server_thread_new("23483", 0);
    lo_server_thread_add_method(osc_server, "/control", "if", 
                                &control_callback, &lv2h);
    lo_server_thread_start(osc_server);
    
    cerr<<"Listening on URL <"<<lo_server_thread_get_url(osc_server)<<">"<<endl;
    
    // wait until we are killed
    while (true) {
      usleep(500000);
      
      lv2h.configure("hello", "world");
      
    }
    
    lo_server_thread_stop(osc_server);
    jack_client_close(jack_client);
    lv2h.deactivate();
  }
  
  else {
    cerr<<"Could not find the plugin with URI <"<<argv[1]<<">"<<endl;
    return 1;
  }
  
  return 0;
}
