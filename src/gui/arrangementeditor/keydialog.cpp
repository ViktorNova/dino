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

#include <iomanip>
#include <sstream>

#include "keydialog.hpp"


//using namespace Dino;
using namespace Glib;
using namespace Gtk;
using namespace std;


KeyDialog::KeyDialog()
  : m_info(255, "") {
  
  set_title("Key properties");
  
  ostringstream oss;
  for (unsigned i = 0; i < 128; ++i) {
    static const char* keys[] = { "C ", "C#", "D ", "D#", "E ", "F ", 
				  "F#", "G ", "G#", "A ", "A#", "B " };
    oss<<setw(3)<<setfill('0')<<i<<": "<<keys[i % 12]<<(i / 12);
    m_cmb_key.append_text(oss.str(), i);
    oss.str("");
  }
  
  //m_cmb_key.signal_changed().
  //  connect(mem_fun(*this, &KeyDialog::update_entry));
  m_ent_name.select_region(0, -1);
  m_cmb_key.set_active_id(1);

  Table* table = manage(new Table(2, 2));
  table->attach(*manage(new Label("Name:")), 0, 1, 0, 1);
  table->attach(*manage(new Label("MIDI key:")), 0, 1, 1, 2);
  table->attach(m_ent_name, 1, 2, 0, 1);
  table->attach(m_cmb_key, 1, 2, 1, 2);
  table->set_border_width(5);
  table->set_row_spacings(5);
  table->set_col_spacings(5);
  get_vbox()->pack_start(*table);
  add_button(Stock::CANCEL, RESPONSE_CANCEL);
  add_button(Stock::OK, RESPONSE_OK);

  //update_entry();
}


const Dino::KeyInfo& KeyDialog::get_info() const {
  m_info.set_name(m_ent_name.get_text());
  m_info.set_number(m_cmb_key.get_active_id());
  return m_info;
}


void KeyDialog::set_info(const Dino::KeyInfo& info) {
  m_info = info;
  m_cmb_key.set_active_id(m_info.get_number());
  m_ent_name.set_text(m_info.get_name());
}


void KeyDialog::refocus() {
  m_ent_name.select_region(0, -1);
  m_ent_name.grab_focus();
}


void KeyDialog::reset() {
  //set_info(ControllerInfo(make_pbend(), "Pitchbend"));
}


/*
void KeyDialog::update_entry() {
  int a, b;
  m_ent_name.get_selection_bounds(a, b);
  if (a == 0 && b == (int)m_ent_name.get_text().size()) {
    if (Dino::is_cc(m_cmb_controller.get_active_id()))
      m_ent_name.
	set_text(m_cc_desc[Dino::cc_number(m_cmb_controller.get_active_id())]);
    else if (Dino::is_pbend(m_cmb_controller.get_active_id()))
      m_ent_name.set_text("Pitchbend");
    m_ent_name.select_region(0, -1);
  }
}
*/
