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


void PluginInterfaceImplementation::add_action(::Action& action) {
  m_actions.insert(&action);
  m_signal_action_added(action);
}


void PluginInterfaceImplementation::remove_action(::Action& action) {
  m_actions.erase(&action);
  m_signal_action_removed(action);
}

 
Dino::Song& PluginInterfaceImplementation::get_song() {
  return m_song;
}


Dino::Sequencer& PluginInterfaceImplementation::get_sequencer() {
  return m_seq;
}


PluginInterface::action_iterator 
PluginInterfaceImplementation::actions_begin() {
  return m_actions.begin();
}


PluginInterface::action_iterator PluginInterfaceImplementation::actions_end() {
  return m_actions.end();
}


sigc::signal<void, ::Action&>& 
PluginInterfaceImplementation::signal_action_added() {
  return m_signal_action_added;
}


sigc::signal<void, ::Action&>& 
PluginInterfaceImplementation::signal_action_removed() {
  return m_signal_action_removed;
}
