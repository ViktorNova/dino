/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <larsl@users.sourceforge.net>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#ifndef CONTROLLERDIALOG_HPP
#define CONTROLLERDIALOG_HPP

#include <gtkmm.h>
#include <libglademm.h>

#include "singletextcombo.hpp"


class ControllerDialog : public Gtk::Dialog {
public:
  
  ControllerDialog(BaseObjectType* obj, 
		   const Glib::RefPtr<Gnome::Glade::Xml>& xml);
  
  std::string get_name() const;
  long get_controller() const;
  
  void set_name(const std::string& name);
  void set_controller(long controller);
  
  void refocus();
  
protected:
  
  void update_entry();
  
  Gtk::Entry* m_ent_name;
  SingleTextCombo m_cmb_controller;

  static std::string m_cc_desc[];
};


#endif
