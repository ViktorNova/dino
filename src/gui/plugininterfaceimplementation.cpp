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

#include <iostream>

#include "dinogui.hpp"
#include "plugininterfaceimplementation.hpp"


PluginInterfaceImplementation::
PluginInterfaceImplementation(DinoGUI& gui, Dino::Song& song, 
			      Dino::Sequencer& sequencer,
			      Dino::CommandProxy& proxy,
			      const std::string& dbus_name)
  : m_gui(gui),
    m_song(song),
    m_seq(sequencer),
    m_proxy(proxy),
    m_dbus_name(dbus_name) {

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

 
const Dino::Song& PluginInterfaceImplementation::get_song() {
  return m_song;
}


Dino::Sequencer& PluginInterfaceImplementation::get_sequencer() {
  return m_seq;
}


Dino::CommandProxy& PluginInterfaceImplementation::get_command_proxy() {
  return m_proxy;
}

const std::string& PluginInterfaceImplementation::get_dbus_name() const {
  return m_dbus_name;
}


PluginInterface::action_iterator 
PluginInterfaceImplementation::actions_begin() {
  return m_actions.begin();
}


PluginInterface::action_iterator PluginInterfaceImplementation::actions_end() {
  return m_actions.end();
}


unsigned PluginInterfaceImplementation::set_status(const std::string& status,
                                                   int timeout) {
  return m_gui.set_status(status, timeout);
}


sigc::signal<void, ::Action&>& 
PluginInterfaceImplementation::signal_action_added() {
  return m_signal_action_added;
}


sigc::signal<void, ::Action&>& 
PluginInterfaceImplementation::signal_action_removed() {
  return m_signal_action_removed;
}


