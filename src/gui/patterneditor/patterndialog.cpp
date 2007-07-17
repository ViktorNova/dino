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

#include "patterndialog.hpp"


using namespace Gtk;
using namespace Glib;


PatternDialog::PatternDialog() {
  set_title("Pattern properties");
  m_ent_name = manage(new Entry);
  m_sbn_length = manage(new SpinButton);
  m_sbn_length->set_range(1, 10000);
  m_sbn_length->set_increments(1, 10);
  m_sbn_steps = manage(new SpinButton);
  m_sbn_steps->set_range(1, 10000);
  m_sbn_steps->set_increments(1, 10);
  Table* table = manage(new Table(3, 2));
  table->set_border_width(5);
  table->set_row_spacings(5);
  table->set_col_spacings(5);
  table->attach(*manage(new Label("Pattern name:")), 0, 1, 0, 1);
  table->attach(*manage(new Label("Pattern length (beats):")), 0, 1, 1, 2);
  table->attach(*manage(new Label("Note steps per beat:")), 0, 1, 2, 3);
  table->attach(*m_ent_name, 1, 2, 0, 1);
  table->attach(*m_sbn_length, 1, 2, 1, 2);
  table->attach(*m_sbn_steps, 1, 2, 2, 3);
  get_vbox()->pack_start(*table);
  add_button(Stock::CANCEL, RESPONSE_CANCEL);
  add_button(Stock::OK, RESPONSE_OK);
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
