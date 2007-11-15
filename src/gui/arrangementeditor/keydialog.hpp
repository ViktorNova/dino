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

#ifndef KEYDIALOG_HPP
#define KEYDIALOG_HPP

#include <gtkmm.h>

#include "keyinfo.hpp"
#include "singletextcombo.hpp"


class KeyDialog : public Gtk::Dialog {
public:
  
  KeyDialog();
  
  const Dino::KeyInfo& get_info() const;
  
  void set_info(const Dino::KeyInfo& info);
  
  void refocus();
  
  void reset();
  
protected:
  
  //void update_entry();
  
  Gtk::Entry m_ent_name;
  mutable Dino::KeyInfo m_info;
  SingleTextCombo m_cmb_key;
};


#endif
