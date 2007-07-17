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

  /** Set the string in the status bar. */
  virtual unsigned set_status(const std::string& status, int timeout);

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
