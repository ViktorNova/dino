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

#ifndef PATTERNDIALOG_HPP
#define PATTERNDIALOG_HPP

#include <gtkmm.h>

#include "singletextcombo.hpp"


class PatternDialog : public Gtk::Dialog {
public:
  
  PatternDialog();
  
  std::string get_name() const;
  int get_length() const;
  int get_steps() const;
  
  void set_name(const std::string& name);
  void set_length(int length);
  void set_steps(int length);

  void refocus();
  
protected:

  Gtk::Entry* m_ent_name;
  Gtk::SpinButton* m_sbn_length;
  Gtk::SpinButton* m_sbn_steps;

};


#endif
