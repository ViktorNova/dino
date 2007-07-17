/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, 
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include <cstdlib>
#include <iostream>

#include "note.hpp"
#include "pattern.hpp"
#include "patternselection.hpp"
#include "plugininterface.hpp"
#include "sequencer.hpp"
#include "song.hpp"
#include "track.hpp"


using namespace Dino;
using namespace std;


namespace {
  
  
  // Clear the loop points
  struct ClearLoopAction : public SongAction {
    std::string get_name() const { return "Clear loop"; }
    void run(Song& song) {
      song.set_loop_end(-1);
      song.set_loop_start(-1);
    }
  }* clearloop;
  
  
  // Set the recording track to 0
  struct StopRecordingAction : SongAction {
    StopRecordingAction(PluginInterface& plif)
      : m_seq(plif.get_sequencer()),
        m_song(plif.get_song()){

    }
    std::string get_name() const { return "Stop recording"; }
    void run(Song& song) {
      m_seq.record_to_track(m_song.tracks_end());
    }
    Song& m_song;
    Sequencer& m_seq;
  }* stoprecording;
  
  
  // Change the track to record to
  struct RecordToTrackAction : public TrackAction {
    RecordToTrackAction(PluginInterface& plif) 
      : m_song(plif.get_song()),
        m_seq(plif.get_sequencer()) { 
    
    }
    std::string get_name() const { return "Record to track"; }
    void run(Track& track) {
      Song::TrackIterator iter = m_song.tracks_find(track.get_id());
      if (iter != m_song.tracks_end())
        m_seq.record_to_track(iter);
    }
    Song& m_song;
    Sequencer& m_seq;
  }* recordtotrack;
  
  // Interpolate the velocity for a pattern selection
  struct InterpolateVelocityAction : public PatternSelectionAction {
    std::string get_name() const { return "Interpolate velocity"; }
    void run(PatternSelection& selection) {
      unsigned int start_step, end_step;
      unsigned char start_vel, end_vel;
      PatternSelection::Iterator iter = selection.begin();
      if (iter == selection.end())
        return;
      start_step = iter->get_step();
      start_vel = iter->get_velocity();
      for ( ; iter != selection.end(); ++iter) {
        end_step = iter->get_step();
        end_vel = iter->get_velocity();
      }
      for (iter = selection.begin(); iter != selection.end(); ++iter) {
        double phase = double(iter->get_step() - start_step) / 
          (end_step - start_step);
        unsigned char vel = (unsigned char)(start_vel + (end_vel - start_vel) *
                                            phase);
        selection.get_pattern()->set_velocity(iter, vel);
      }
    }
  }* interpolatevelocity;
  
  
  // Randomise the velocities for the selected notes
  struct RandomiseVelocityAction : public PatternSelectionAction {
    std::string get_name() const { return "Randomise velocity"; }
    void run(PatternSelection& selection) {
      unsigned char min_vel = 127;
      unsigned char max_vel = 1;
      PatternSelection::Iterator iter = selection.begin();
      if (iter == selection.end())
        return;
      for ( ; iter != selection.end(); ++iter) {
        unsigned char vel = iter->get_velocity();
        min_vel = vel < min_vel ? vel : min_vel;
        max_vel = vel > max_vel ? vel : max_vel;
      }
      for (iter = selection.begin(); iter != selection.end(); ++iter) {
        selection.get_pattern()->set_velocity(iter, min_vel + rand() % 
                                              (max_vel - min_vel + 1));
      }
    }
  }* randomisevelocity;
  
  
  PluginInterface* m_plif = 0;
}


extern "C" {
  string dino_get_name() { 
    return "Core actions"; 
  }
  
  void dino_load_plugin(PluginInterface& plif) {
    plif.add_action(*(clearloop = new ClearLoopAction));
    plif.add_action(*(stoprecording = new StopRecordingAction(plif)));
    plif.add_action(*(interpolatevelocity = new InterpolateVelocityAction));
    plif.add_action(*(randomisevelocity = new RandomiseVelocityAction));
    plif.add_action(*(recordtotrack = new RecordToTrackAction(plif)));
    m_plif = &plif;
  }
  
  void dino_unload_plugin() {
    m_plif->remove_action(*clearloop);
    m_plif->remove_action(*stoprecording);
    m_plif->remove_action(*interpolatevelocity);
    m_plif->remove_action(*randomisevelocity);
    m_plif->remove_action(*recordtotrack);
    delete clearloop;
  }
}


