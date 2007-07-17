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

#ifndef PLUGINDIALOG_HPP
#define PLUGINDIALOG_HPP

#include <gtkmm.h>

#include "pluginlibrary.hpp"


class PluginDialog : public Gtk::Dialog {
public:
  
  PluginDialog();
  
  void set_library(PluginLibrary& plib);
  
protected:
  
  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    Gtk::TreeModelColumn<bool> loaded;
    Gtk::TreeModelColumn<std::string> name;
    Gtk::TreeModelColumn<std::string> filename;
    
    ModelColumns() { 
      add(loaded);
      add(name); 
      add(filename); 
    }
  } m_columns;

  
  void row_changed(const Gtk::TreeModel::iterator& iter);
  void loaded(PluginLibrary::iterator iter);
  void unloaded(PluginLibrary::iterator iter);
  
  
  Glib::RefPtr<Gtk::ListStore> m_store;
  
  PluginLibrary* m_plib;
  
  Gtk::TreeView m_view;
};


#endif
