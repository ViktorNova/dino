/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, 
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#ifndef CONTROLLERDIALOG_HPP
#define CONTROLLERDIALOG_HPP

#include <gtkmm.h>

#include "controllerinfo.hpp"
#include "singletextcombo.hpp"


class ControllerDialog : public Gtk::Dialog {
public:
  
  ControllerDialog();
  
  const Dino::ControllerInfo& get_info() const;
  
  void set_info(const Dino::ControllerInfo& info);
  
  void refocus();
  
  void reset();
  
protected:
  
  void update_entry();
  
  Gtk::Entry* m_ent_name;
  SingleTextCombo m_cmb_controller;
  
  mutable Dino::ControllerInfo m_info;
  
  static std::string m_cc_desc[];
};


#endif
