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
  
  /** Returns the used Dino::Song object. */
  Dino::Song& get_song();
  
  /** Returns the used Dino::Sequencer object. */
  Dino::Sequencer& get_sequencer();
  
protected:
  
  DinoGUI& m_gui;
  Dino::Song& m_song;
  Dino::Sequencer& m_seq;
  
};


#endif
