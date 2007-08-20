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

#ifndef PLUGININTERFACE_HPP
#define PLUGININTERFACE_HPP

#include <iterator>
#include <string>
#include <set>

#include <gtkmm.h>

#include "action.hpp"


namespace Dino {
  class Song;
  class Sequencer;
  class CommandProxy;
}


class GUIPage : public Gtk::HBox {
public:
  
  enum Flags {
    PageNoFlags = 0,
    PageSupportsClipboard = (1 << 0)
  };
  
  GUIPage(Flags flags = PageNoFlags) : m_flags(flags) { }
  
  Flags get_flags() const { return m_flags; }
  
  virtual void cut_selection() { }
  virtual void copy_selection() { }
  virtual void paste() { }
  virtual void delete_selection() { }
  virtual void select_all() { }
  
  virtual void reset_gui() { }
  
private:
  
  Flags m_flags;
  
};


/** An object of this class is passed to Plugin::initialise() when Dino loads
    a plugin. It can be used to access the functions and data structures
    available to the plugin. */
class PluginInterface {
public:
  
  typedef std::set< ::Action*>::iterator action_iterator;
  
  virtual ~PluginInterface() { }
  
  /** Add a page to the main notebook. */
  virtual void add_page(const std::string& label, GUIPage& widget) = 0;

  /** Remove a page from the main notebook. */
  virtual void remove_page(GUIPage& widget) = 0;
  
  /** Add an action. */
  virtual void add_action(::Action& action) = 0;
  
  /** Remove an action. */
  virtual void remove_action(::Action& action) = 0;
  
  /** Returns the Dino::CommandProxy object. */
  virtual Dino::CommandProxy& get_command_proxy() = 0;
  
  /** Returns the used Dino::Sequencer object. */
  virtual Dino::Sequencer& get_sequencer() = 0;
  
  /** Returns the D-Bus connection name. */
  virtual const std::string& get_dbus_name() const = 0;
  
  /** Returns an iterator pointing to the beginning of the action list. */
  virtual action_iterator actions_begin() = 0;

  /** Returns an iterator pointing to the end of the action list. */
  virtual action_iterator actions_end() = 0;
  
  /** Sets the string in the statusbar. */
  virtual unsigned set_status(const std::string& status, int timeout = 3000) = 0;
  
  virtual sigc::signal<void, ::Action&>& signal_action_added() = 0;
  virtual sigc::signal<void, ::Action&>& signal_action_removed() = 0;
  
};


typedef std::string (*PluginNameFunc)(void);
typedef void (*PluginLoadFunc)(PluginInterface&);
typedef void (*PluginUnloadFunc)(void);


#endif
