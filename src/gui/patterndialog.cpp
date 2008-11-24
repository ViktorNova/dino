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

#include "patterndialog.hpp"


using namespace Gtk;
using namespace Glib;
using namespace Gnome::Glade;


PatternDialog::PatternDialog(BaseObjectType* obj, const RefPtr<Xml>& xml) 
  : Dialog(obj) {
  m_ent_name = dynamic_cast<Entry*>(xml->get_widget("dlgpat_ent_name"));
  m_sbn_length =dynamic_cast<SpinButton*>(xml->get_widget("dlgpat_sbn_length"));
  m_sbn_steps = dynamic_cast<SpinButton*>(xml->get_widget("dlgpat_sbn_steps"));
  manage(m_ent_name);
  manage(m_sbn_length);
  manage(m_sbn_steps);
}


std::string PatternDialog::get_name() const {
  return m_ent_name->get_text();
}


int PatternDialog::get_length() const {
  return m_sbn_length->get_value_as_int();
}


int PatternDialog::get_steps() const {
  return m_sbn_steps->get_value_as_int();
}

  
void PatternDialog::set_name(const std::string& name) {
  m_ent_name->set_text(name);
}


void PatternDialog::set_length(int length) {
  m_sbn_length->set_value(length);
}


void PatternDialog::set_steps(int steps) {
  m_sbn_steps->set_value(steps);
}


void PatternDialog::refocus() {
  m_ent_name->select_region(0, -1);
  m_ent_name->grab_focus();
}
