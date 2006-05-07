#include <iostream>

#include "dinogui.hpp"
#include "plugininterfaceimplementation.hpp"


PluginInterfaceImplementation::
PluginInterfaceImplementation(DinoGUI& gui, Dino::Song& song, 
			      Dino::Sequencer& sequencer)
  : m_gui(gui),
    m_song(song),
    m_seq(sequencer) {

}


void PluginInterfaceImplementation::add_page(const std::string& label, 
					     GUIPage& widget) {
  m_gui.add_page(label, widget);
}
  

void PluginInterfaceImplementation::remove_page(GUIPage& widget) {
  m_gui.remove_page(widget);
}

 
Dino::Song& PluginInterfaceImplementation::get_song() {
  return m_song;
}


Dino::Sequencer& PluginInterfaceImplementation::get_sequencer() {
  return m_seq;
}


