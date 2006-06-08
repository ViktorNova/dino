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
    : m_events(1024),
      m_track(0) {
    
    signal_timeout().
      connect(bind_return(mem_fun(*this, &Recorder::do_record), true), 100);
    
  }
    

  void Recorder::set_track(Track* track) {
    m_track = track;
  }
  
  
  Track* Recorder::get_track() {
    return m_track;
  }


  void Recorder::start(double beat) {
    Event e;
    e.type = EVENT_START;
    e.beat1 = beat;
    m_events.push(e);
  }


  void Recorder::record_event(double beat, unsigned int size, 
                              unsigned char* data) {
    Event e;
    e.type = EVENT_MIDI;
    e.beat1 = beat;
    e.size = size;
    memcpy(e.data, data, e.size);
    m_events.push(e);
  }


  void Recorder::relocate(double from, double to) {
    Event e;
    e.type = EVENT_RELOCATE;
    e.beat1 = from;
    e.beat2 = to;
    m_events.push(e);
  }


  void Recorder::stop(double beat) {
    Event e;
    e.type = EVENT_STOP;
    e.beat1 = beat;
    m_events.push(e);
  }
    

  void Recorder::do_record() {
    if (!m_track)
      return;
    Event e;
    while (m_events.pop(e)) {
      switch (e.type) {
        
      case EVENT_START:
        dbg1<<"Recorded EVENT_START at "<<e.beat1<<endl;
        break;
        
      case EVENT_MIDI:
        dbg1<<"Recorded EVENT_MIDI at "<<e.beat1<<endl;
        dbg1<<"size = "<<e.size<<endl;
        dbg1<<"data: ";
        for (int i = 0; i < e.size; ++i)
          cerr<<" 0x"<<hex<<int(e.data[i])<<dec;
        cerr<<endl;
        
        if (e.size > 0) {
          
          // note on
          if (e.size >= 3 && (e.data[0] & 0xF0) == 0x90 && e.data[2] > 0) {
            dbg1<<"NOTE ON"<<endl;
            int key = e.data[1];
            if (m_notes[key].valid)
              record_note(key, e.beat1);
            Track::SequenceIterator iter = m_track->seq_find(int(e.beat1));
            if (iter != m_track->seq_end()) {
              m_notes[key].valid = true;
              m_notes[key].start = e.beat1;
              m_notes[key].velocity = e.data[2];
              m_notes[key].pattern_id = iter->get_pattern_id();
            }
          }
          
          // note off
          else if (e.size >= 3 && ((e.data[0] & 0xF0) == 0x80 || 
                                   ((e.data[0] & 0xF0) == 0x90 && 
                                    e.data[2] == 0))) {
            dbg1<<"NOTE OFF"<<endl;
            int key = e.data[1];
            if (m_notes[key].valid)
              record_note(key, e.beat1);
          }
          
          // pitchbend
          else if(e.size >= 3 && (e.data[0] & 0xF0) == 0xE0) {
            int value = e.data[1] + (e.data[2] << 7) - 8192;
            dbg1<<"PITCHBEND "<<value<<endl;
            unsigned beat = unsigned(e.beat1);
            Track::SequenceIterator iter = m_track->seq_find(beat);
            if (iter != m_track->seq_end()) {
              if (beat < iter->get_start() + iter->get_length()) {
                Pattern& pat = iter->get_pattern();
                unsigned step = unsigned((e.beat1 - iter->get_start()) * 
                                         pat.get_steps() + 0.5);
                assert(step <= iter->get_length() * pat.get_steps());
                Pattern::ControllerIterator citer;
                citer = pat.ctrls_find(make_pbend());
                if (citer == pat.ctrls_end())
                  citer = pat.add_controller("Pitchbend", make_pbend(), 
                                              -8192, 8191);
                pat.add_cc(citer, step, value);
              }
            }
          }
          
        }
        break;
        
      case EVENT_STOP:
        dbg1<<"Recorded EVENT_STOP at "<<e.beat1<<endl;
        for (unsigned i = 0; i < 128; ++i) {
          if (m_notes[i].valid)
            record_note(i, e.beat1);
        }
        break;
        
      case EVENT_RELOCATE:
        dbg1<<"Recorded EVENT_RELOCATE from "<<e.beat1<<" to "<<e.beat2<<endl;
        for (unsigned i = 0; i < 128; ++i) {
          if (m_notes[i].valid)
            record_note(i, e.beat1);
        }
        break;
        
      }
    }
  }

  
  void Recorder::record_note(unsigned char key, double end) {
    dbg1<<"Recording note from "<<m_notes[key].start<<" to "<<end<<endl;
    m_notes[key].valid = false;
    Track::SequenceIterator siter = m_track->seq_find(int(m_notes[key].start));
    if (siter == m_track->seq_end())
      return;
    double start = m_notes[key].start;
    if (start < siter->get_start() || 
        start >= siter->get_start() + siter->get_length())
      return;
    if (end > siter->get_start() + siter->get_length())
      end = siter->get_start() + siter->get_length();
    Pattern& pat = siter->get_pattern();
    unsigned int offset = siter->get_start();
    unsigned int steps = pat.get_steps();
    unsigned int start_step = unsigned((start - offset) * steps + 0.5);
    unsigned int end_step = unsigned((end - offset) * steps + 0.5);
    if (start_step >= pat.get_steps() * pat.get_length())
      return;
    if (end_step <= start_step)
      end_step = start_step + 1;
    pat.add_note(start_step, key, m_notes[key].velocity, 
                 end_step - start_step);
  }
  
  
}

