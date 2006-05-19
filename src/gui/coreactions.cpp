#include <cstdlib>
#include <iostream>

#include "plugininterface.hpp"
#include "song.hpp"
#include "pattern.hpp"
#include "patternselection.hpp"
#include "note.hpp"


using namespace Dino;


namespace {
  
  
  // Clear the loop points
  struct ClearLoopAction : public SongAction {
    std::string get_name() const { return "Clear loop"; }
    void run(Song& song) {
      song.set_loop_end(-1);
      song.set_loop_start(-1);
    }
  }* clearloop;
  
  
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
    plif.add_action(*(interpolatevelocity = new InterpolateVelocityAction));
    plif.add_action(*(randomisevelocity = new RandomiseVelocityAction));
    m_plif = &plif;
  }
  
  void dino_unload_plugin() {
    m_plif->remove_action(*clearloop);
    m_plif->remove_action(*interpolatevelocity);
    m_plif->remove_action(*randomisevelocity);
    delete clearloop;
  }
}


