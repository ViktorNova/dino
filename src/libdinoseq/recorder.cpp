#include <cstring>
#include <iostream>

#include <glibmm.h>

#include "controller_numbers.hpp"
#include "debug.hpp"
#include "pattern.hpp"
#include "recorder.hpp"
#include "song.hpp"
#include "track.hpp"

#include <jack/midiport.h>


using namespace Glib;
using namespace sigc;
using namespace std;


namespace Dino {


  Recorder::Recorder(Song& song) 
    : m_track(0),
      m_song(song),
      m_last_sid(-1),
      m_track_set(false),
      m_last_beat(-666),
      m_last_state(JackTransportStopped),
      m_to_audio(1024),
      m_from_audio(1024),
      m_expected_beat(0),
      m_expected_frame(0) {
    
  }
    

  void Recorder::set_track(int id) {
    Command c = { CommandTypeSetTrack };
    c.set_track.track = id;
    m_to_audio.push(c);
  }
  
  
  int Recorder::get_track() {
    return m_track;
  }


  void Recorder::run_editing_thread() {
    
    Song::TrackIterator titer = m_song.tracks_find(m_track);
    
    Event e;
    while (m_from_audio.pop(e)) {
      
      // if we are in a different sequence entry, stop all notes in the last
      int sid = -1;
      if (titer != m_song.tracks_end()) {
        Track::SequenceIterator siter = titer->seq_find(int(e.beat));
        if (siter != titer->seq_end())
          sid = siter->get_id();
        cerr<<"m_last_sid = "<<m_last_sid<<", sid = "<<sid<<endl;
        if (sid != m_last_sid) {
          for (int i = 0; i < 128; ++i) {
            if (m_keys[i].held)
              handle_note_off(e.beat, i, *titer);
          }
        }
      }
      m_last_sid = sid;
      
      switch (e.type) {
        
      case EventTypeTrackSet:
        if (e.track_set.track != m_track) {
          if (titer != m_song.tracks_end()) {
            for (int i = 0; i < 128; ++i) {
              if (m_keys[i].held)
                handle_note_off(e.beat, i, *titer);
            }
          }
          titer = m_song.tracks_find(e.track_set.track);
          if (titer != m_song.tracks_end())
            m_track = e.track_set.track;
          else
            m_track = 0;
          signal_track_set(m_track);
          cerr<<"SET TRACK TO "<<m_track<<" AT "<<e.beat<<endl;
        }
        break;
        
      case EventTypeBeat:
        cerr<<"BEAT: "<<e.beat<<endl;
        break;
        
      case EventTypeStart:
        cerr<<"START: "<<e.beat<<endl;
        break;

      case EventTypeStop:
        cerr<<"STOP: "<<e.beat<<endl;
        if (titer != m_song.tracks_end()) {
          for (int i = 0; i < 128; ++i) {
            if (m_keys[i].held)
              handle_note_off(e.beat, i, *titer);
          }
        }
        break;
        
      case EventTypeRelocation:
        cerr<<"RELOCATION: "<<e.beat<<" -> "<<e.relocation.to<<endl;
        if (titer != m_song.tracks_end()) {
          for (int i = 0; i < 128; ++i) {
            if (m_keys[i].held)
              handle_note_off(e.beat, i, *titer);
          }
        }
        break;
        
      case EventTypeMIDI:
        cerr<<"MIDI at "<<e.beat<<':';
        for (unsigned int i = 0; i < e.midi.size; ++i)
          cerr<<" "<<hex<<setw(2)<<setfill('0')<<int(e.midi.data[i]);
        cerr<<dec<<endl;
        
        if (titer != m_song.tracks_end()) {
          unsigned char* data = e.midi.data;
          unsigned char status = data[0] & 0xF0;
          
          // NOTE ON
          if (status == 0x90 && data[2] != 0)
            handle_note_on(e.beat, data[1], data[2], *titer);
          
          // NOTE OFF
          else if (status == 0x80 || (status == 0x90 && data[2] == 0))
            handle_note_off(e.beat, data[1], *titer);
          
          // CONTROLLER
          else if (status == 0xB0)
            handle_controller(e.beat, make_cc(data[1]), data[2], *titer);
          
          // PITCHBEND
          else if (status == 0xE0)
            handle_controller(e.beat, make_pbend(), 
                              data[1] + (data[2] << 7) - 0x2000, *titer);
        }
        
        break;
        
      }
    }
  }
    
  
  void Recorder::run_audio_thread(jack_transport_state_t state, 
                                  const jack_position_t& pos, 
                                  jack_nframes_t nframes, void* input_buf,
                                  jack_nframes_t rate) {
    // get the current time
    double start_beat = pos.bar * pos.beats_per_bar + pos.beat + 
      pos.tick / pos.ticks_per_beat;
    double beats_per_frame = pos.beats_per_minute / (60 * rate);
    
    // read commands from the editing thread
    Command c;
    while (m_to_audio.pop(c)) {
      switch (c.type) {
      case CommandTypeSetTrack:
        m_track_set = (c.set_track.track != 0);
        Event e = { EventTypeTrackSet, start_beat };
        e.track_set.track = c.set_track.track;
        m_from_audio.push(e);
        break;
      }
    }
    
    // started or stopped or relocated?
    if (m_last_state == JackTransportRolling && state != JackTransportRolling) {
      Event e = { EventTypeStop, m_expected_beat };
      m_from_audio.push(e);
    }
    if (m_expected_frame != pos.frame) {
      Event e = { EventTypeRelocation, m_expected_beat };
      e.relocation.to = start_beat;
      m_from_audio.push(e);
    }
    if (m_last_state != JackTransportRolling && state == JackTransportRolling) {
      Event e = { EventTypeStart, start_beat };
      m_from_audio.push(e);
    }
    m_last_state = state;


    m_expected_beat = start_beat + (state == JackTransportRolling ?
                                    nframes * beats_per_frame : 0);
    m_expected_frame = pos.frame + (state == JackTransportRolling ? 
                                    nframes : 0);
    
    // if we're not recording, don't do anything else
    if (state != JackTransportRolling || !m_track_set)
      return;

    // send new beat
    if (int(start_beat) != m_last_beat) {
      Event e = { EventTypeBeat, start_beat };
      m_from_audio.push(e);
      m_last_beat = int(start_beat);
    }
    
    // send MIDI events
   jack_midi_event_t input_event;
   jack_nframes_t input_event_index = 0;
   jack_nframes_t input_event_count = 
     jack_midi_port_get_info(input_buf, nframes)->event_count;
   double bpf = pos.beats_per_minute / (60 * pos.frame_rate);
   for (unsigned int i = 0; i < input_event_count; ++i) {
     jack_midi_event_get(&input_event, input_buf, i, nframes);
     if (input_event.size <= 3) {
       double beat = start_beat + input_event.time * beats_per_frame;
       Event e = { EventTypeMIDI, beat };
       e.midi.size = input_event.size;
       memcpy(e.midi.data, input_event.buffer, input_event.size);
       m_from_audio.push(e);
     }
   }
    
  }


  void Recorder::handle_note_on(double beat, unsigned char key, 
                                unsigned char velocity, Track& track) {
    Track::SequenceIterator siter = track.seq_find(int(beat));
    if (siter != track.seq_end()) {
      Key& k = m_keys[key];
      k.held = true;
      k.pattern = siter->get_pattern_id();
      k.start = beat - siter->get_start();
      k.velocity = velocity;
      k.seq_id = siter->get_id();
      cerr<<"NOTE ON in pattern "<<k.pattern<<" at beat "<<k.start
          <<" with velocity "<<int(k.velocity)<<endl;
    }
  }

  
  void Recorder::handle_note_off(double beat, unsigned char key, Track& track) {
    Key& k = m_keys[key];
    // XXX shouldn't use the ID, should connect to the seq_entry_removed 
    // signal instead
    Track::SequenceIterator siter = track.seq_find_by_id(k.seq_id);
    if (k.held && siter != track.seq_end()) {
      
      // only continue if the pattern still exists
      Track::PatternIterator piter = track.pat_find(k.pattern);
      if (piter != track.pat_end()) {
        
        double end = beat - siter->get_start();
        Pattern& pat = *piter;
        
        // only continue if the start beat is still inside the pattern
        if (k.start < pat.get_length() * pat.get_steps()) {
          
          int start_step = int(k.start * pat.get_steps() + 0.5);
          int end_step = int(end * pat.get_steps() + 0.5);
          
          if (end_step >= start_step) {
            if (start_step >= pat.get_length() * pat.get_steps())
              start_step = pat.get_length() * pat.get_steps() - 1;
            if (end_step == start_step)
              end_step = start_step + 1;
            if (end_step > pat.get_length() * pat.get_steps())
              end_step = pat.get_length() * pat.get_steps();
            
            // delete any notes that start during this note
            for (unsigned int s = start_step; s < end_step; ++s) {
              Pattern::NoteIterator niter = pat.find_note(s, key);
              if (niter != pat.notes_end())
                pat.delete_note(niter);
            }
            
            pat.add_note(start_step, key, k.velocity, end_step - start_step);
          }
        }
      }
    }
    
    k.held = false;
  }
  
  
  void Recorder::handle_controller(double beat, long param, int value, 
                                   Track& track) {
    Track::SequenceIterator siter = track.seq_find(int(beat));
    if (siter != track.seq_end()) {
      Pattern& pat = siter->get_pattern();
      Pattern::ControllerIterator citer = pat.ctrls_find(param);
      if (citer != pat.ctrls_end()) {
        unsigned long step = (unsigned long)((beat - siter->get_start()) * 
                                             pat.get_steps());
        pat.add_cc(citer, step, value);
      }
    }
  }

}

