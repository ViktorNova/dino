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

#include "sequencer.hpp"
#include "trackdialog.hpp"


using namespace Gtk;
using namespace Glib;
using namespace std;
using namespace Dino;


TrackDialog::TrackDialog() {
  set_title("Track properties");
  m_ent_name = manage(new Entry);
  m_sbn_channel = manage(new SpinButton(0, 0));
  m_sbn_channel->set_range(1, 16);
  m_sbn_channel->set_increments(1, 10);
  Table* table = manage(new Table(3, 2));
  table->attach(*manage(new Label("Track name:")), 0, 1, 0, 1);
  table->attach(*manage(new Label("MIDI port:")), 0, 1, 1, 2);
  table->attach(*manage(new Label("MIDI channel:")), 0, 1, 2, 3);
  table->attach(*m_ent_name, 1, 2, 0, 1);
  table->attach(m_cmb_port, 1, 2, 1, 2);
  table->attach(*m_sbn_channel, 1, 2, 2, 3);
  table->set_border_width(5);
  table->set_row_spacings(5);
  table->set_col_spacings(5);
  get_vbox()->pack_start(*table);
  add_button(Stock::CANCEL, RESPONSE_CANCEL);
  add_button(Stock::OK, RESPONSE_OK);
  
  update_ports();
}


std::string TrackDialog::get_name() const {
  return m_ent_name->get_text();
}


std::string TrackDialog::get_port() const {
  return m_cmb_port.get_active_text();
}


int TrackDialog::get_channel() const {
  return m_sbn_channel->get_value_as_int();
}


void TrackDialog::set_name(const std::string& name) {
  m_ent_name->set_text(name);
}


void TrackDialog::set_channel(int channel) {
  m_sbn_channel->set_value(channel);
}


void TrackDialog::update_ports(const Dino::Sequencer* seq) {
  m_cmb_port.clear();
  m_cmb_port.append_text("None", -1);
  m_cmb_port.set_active_id(-1);
  if (seq != 0) {
    vector<Sequencer::InstrumentInfo> info = seq->get_instruments();
    for (size_t i = 0; i < info.size(); ++i)
      m_cmb_port.append_text(info[i].name, i);
  }
}


void TrackDialog::refocus() {
  m_ent_name->select_region(0, -1);
  m_ent_name->grab_focus();
}
