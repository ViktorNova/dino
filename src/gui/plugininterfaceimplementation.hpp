#ifndef PLUGININTERFACEIMPLEMENTATION_HPP
#define PLUGININTERFACEIMPLEMENTATION_HPP

#include "plugininterface.hpp"


class DinoGUI;


/** An object of this class is passed to Plugin::initialise() when Dino loads
    a plugin. It can be used to access the functions and data structures
    available to the plugin. */
class PluginInterfaceImplementation : public PluginInterface {
public:
  
  PluginInterfaceImplementation(DinoGUI& gui, Dino::Song& song, 
				Dino::Sequencer& sequencer);
  
  /** Add a page to the main notebook. */
  void add_page(const std::string& label, GUIPage& widget);
  
  /** Remove a page from the main notebook. */
  void remove_page(GUIPage& widget);

  /** Add an action. */
  void add_action(::Action& action);
  
  /** Remove an action. */
  void remove_action(::Action& action);
  
  /** Returns the used Dino::Song object. */
  Dino::Song& get_song();
  
  /** Returns the used Dino::Sequencer object. */
  Dino::Sequencer& get_sequencer();
  
  /** Returns an iterator pointing to the beginning of the action list. */
  virtual action_iterator actions_begin();

  /** Returns an iterator pointing to the end of the action list. */
  virtual action_iterator actions_end();

  virtual sigc::signal<void, ::Action&>& signal_action_added();
  virtual sigc::signal<void, ::Action&>& signal_action_removed();
  
protected:
  
  DinoGUI& m_gui;
  Dino::Song& m_song;
  Dino::Sequencer& m_seq;
  
  std::set< ::Action*> m_actions;
  
  sigc::signal<void, ::Action&> m_signal_action_added;
  sigc::signal<void, ::Action&> m_signal_action_removed;
  
};


#endif
