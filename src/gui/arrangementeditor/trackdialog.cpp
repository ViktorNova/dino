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

#include <cassert>

#include "commandproxy.hpp"
#include "sequencer.hpp"
#include "trackdialog.hpp"
#include "controller_numbers.hpp"
#include "track.hpp"


using namespace Gtk;
using namespace Glib;
using namespace std;
using namespace Dino;


TrackDialog::TrackDialog()
  : m_sbn_channel(0, 0) {
  
  set_title("Track properties");
  m_sbn_channel.set_range(1, 16);
  m_sbn_channel.set_increments(1, 10);
  Table* table = manage(new Table(9, 2));
  table->attach(*manage(new Label("Track name:")), 0, 1, 0, 1);
  table->attach(m_ent_name, 1, 2, 0, 1);
  table->attach(*manage(new Label("MIDI port:")), 0, 1, 1, 2);
  table->attach(m_cmb_port, 1, 2, 1, 2);
  table->attach(*manage(new Label("MIDI channel:")), 0, 1, 2, 3);
  table->attach(m_sbn_channel, 1, 2, 2, 3);
  table->attach(*manage(new HSeparator), 0, 2, 3, 4);
  table->attach(*manage(new Label("Controllers:")), 0, 1, 4, 5);
  table->attach(m_cmb_ctrls, 1, 2, 4, 5);
  HBox* hbox = manage(new HBox(false, 3));
  Button* add_ctrl_btn = manage(new Button("Add"));
  hbox->pack_start(*add_ctrl_btn);
  Button* remove_ctrl_btn = manage(new Button("Remove"));
  hbox->pack_start(*remove_ctrl_btn);
  Button* modify_ctrl_btn = manage(new Button("Modify"));
  hbox->pack_start(*modify_ctrl_btn);
  table->attach(*hbox, 1, 2, 5, 6);
  table->attach(*manage(new HSeparator), 0, 2, 6, 7);
  table->attach(*manage(new Label("Named keys:")), 0, 1, 7, 8);
  table->attach(m_cmb_keys, 1, 2, 7, 8);
  hbox = manage(new HBox(false, 3));
  Button* add_key_btn = manage(new Button("Add"));
  hbox->pack_start(*add_key_btn);
  Button* remove_key_btn = manage(new Button("Remove"));
  hbox->pack_start(*remove_key_btn);
  Button* modify_key_btn = manage(new Button("Modify"));
  hbox->pack_start(*modify_key_btn);
  table->attach(*hbox, 1, 2, 8, 9);
  table->set_border_width(5);
  table->set_row_spacings(5);
  table->set_col_spacings(5);
  get_vbox()->pack_start(*table);
  add_button(Stock::CANCEL, RESPONSE_CANCEL);
  add_button(Stock::OK, RESPONSE_OK);
  
  add_ctrl_btn->signal_clicked().
    connect(mem_fun(*this, &TrackDialog::add_controller_clicked));
  remove_ctrl_btn->signal_clicked().
    connect(mem_fun(*this, &TrackDialog::remove_controller_clicked));
  modify_ctrl_btn->signal_clicked().
    connect(mem_fun(*this, &TrackDialog::modify_controller_clicked));
  add_key_btn->signal_clicked().
    connect(mem_fun(*this, &TrackDialog::add_key_clicked));
  remove_key_btn->signal_clicked().
    connect(mem_fun(*this, &TrackDialog::remove_key_clicked));
  modify_key_btn->signal_clicked().
    connect(mem_fun(*this, &TrackDialog::modify_key_clicked));
  
  update_ports();
}


string TrackDialog::get_name() const {
  return m_ent_name.get_text();
}


string TrackDialog::get_port() const {
  return m_cmb_port.get_active_text();
}


int TrackDialog::get_channel() const {
  return m_sbn_channel.get_value_as_int();
}


void TrackDialog::set_name(const string& name) {
  m_ent_name.set_text(name);
}


void TrackDialog::set_channel(int channel) {
  m_sbn_channel.set_value(channel);
}


void TrackDialog::update_ports(const Dino::Sequencer* seq) {
  m_cmb_port.clear();
  m_cmb_port.append_text("None", -1);
  m_cmb_port.set_active_id(-1);
  if (seq != 0) {
    vector<InstrumentInfo> info = seq->get_instruments();
    for (size_t i = 0; i < info.size(); ++i)
      m_cmb_port.append_text(info[i].get_name(), i);
  }
}


void TrackDialog::set_controllers(const vector<ControllerInfo*>& ctrls) {
  m_ctrls.clear();
  m_cmb_ctrls.clear();
  for (unsigned i = 0; i < ctrls.size(); ++i) {
    m_ctrls.push_back(CIWrapper(ControllerInfo(*ctrls[i])));
    m_cmb_ctrls.append_text(m_ctrls[i].ci.get_name(), 
			    m_ctrls[i].ci.get_number());
  }
}


void TrackDialog::set_keys(const vector<KeyInfo*>& keys) {
  m_keys.clear();
  m_cmb_keys.clear();
  for (unsigned i = 0; i < keys.size(); ++i) {
    m_keys.push_back(KIWrapper(KeyInfo(*keys[i])));
    m_cmb_keys.append_text(m_keys[i].ki.get_name(), 
			    m_keys[i].ki.get_number());
  }
}


void TrackDialog::reset() {
  set_name("Untitled");
  set_channel(1);
  m_cmb_port.set_active_id(-1);
  m_cmb_ctrls.clear();
  m_ctrls.clear();
  refocus();
}


void TrackDialog::set_track(const Dino::Track& track, Dino::Sequencer& seq) {
  set_name(track.get_name());
  set_channel(track.get_channel() + 1);
  set_controllers(track.get_controllers());
  set_keys(track.get_keys());
  vector<InstrumentInfo> info = seq.get_instruments(track.get_id());
  for (unsigned i = 0; i < info.size(); ++i) {
    if (info[i].get_connected()) {
      m_cmb_port.set_active_text(info[i].get_name());
      break;
    }
  }
  refocus();
}


void TrackDialog::apply_to_track(const Dino::Track& t, Dino::Sequencer& seq, 
				 Dino::CommandProxy& proxy) {
  
  proxy.start_atomic("Edit track properties");
  
  // update name and MIDI channel
  if (t.get_name() != get_name())
    proxy.set_track_name(t.get_id(), get_name());
  if (t.get_channel() != get_channel() - 1)
    proxy.set_track_midi_channel(t.get_id(), get_channel() - 1);
  seq.set_instrument(t.get_id(), get_port());
  
  // update controllers
  unsigned ncontrollers = t.get_controllers().size();
  vector<long> numbers;
  for (unsigned i = 0; i < ncontrollers; ++i) {
    if (m_ctrls[i].deleted)
      numbers.push_back(t.get_controllers()[i]->get_number());
    else {
      const ControllerInfo& ci = m_ctrls[i].ci;
      if (ci.get_name() != t.get_controllers()[i]->get_name())
	proxy.set_controller_name(t.get_id(), ci.get_number(), ci.get_name());
      if (ci.get_global() != t.get_controllers()[i]->get_global())
	proxy.set_controller_global(t.get_id(),ci.get_number(),ci.get_global());
      // XXX This should be made realtime safe - the sequencer will read the
      // parameter number from the ControllerInfo struct
      // XXX It will not now, every curve have their own ControllerInfo copy -
      // need to be synced though
      t.get_controllers()[i]->set_number(m_ctrls[i].ci.get_number());
    }
  }
  for (unsigned i = 0; i < numbers.size(); ++i)
    proxy.remove_controller(t.get_id(), numbers[i]);
  
  for (unsigned i = ncontrollers; i < m_ctrls.size(); ++i) {
    proxy.add_controller(t.get_id(), m_ctrls[i].ci.get_number(), 
			 m_ctrls[i].ci.get_name(), m_ctrls[i].ci.get_default(),
			 m_ctrls[i].ci.get_min(), m_ctrls[i].ci.get_max(), 
			 m_ctrls[i].ci.get_global());
  }
  
  // update keys
  unsigned nkeys = t.get_keys().size();
  numbers.clear();
  for (unsigned i = 0; i < nkeys; ++i) {
    if (m_keys[i].deleted)
      numbers.push_back(t.get_keys()[i]->get_number());
    else {
      const KeyInfo& ki = m_keys[i].ki;
      if (ki.get_name() != t.get_keys()[i]->get_name())
	proxy.set_key_name(t.get_id(), ki.get_number(), ki.get_name());
      t.get_keys()[i]->set_number(m_keys[i].ki.get_number());
    }
  }
  for (unsigned i = 0; i < numbers.size(); ++i)
    proxy.remove_key(t.get_id(), numbers[i]);
  
  for (unsigned i = nkeys; i < m_keys.size(); ++i) {
    proxy.add_key(t.get_id(), m_keys[i].ki.get_number(), 
		  m_keys[i].ki.get_name());
  }
  
  proxy.end_atomic();
  
}


void TrackDialog::refocus() {
  m_ent_name.select_region(0, -1);
  m_ent_name.grab_focus();
}


bool TrackDialog::add_controller(const ControllerInfo& info) {
  cerr<<"Adding "<<info.get_number()<<" ("<<info.get_name()<<")"<<endl;
  for (unsigned i = 0; i < m_ctrls.size(); ++i) {
    if (info.get_number() == m_ctrls[i].ci.get_number())
      return false;
  }
  m_ctrls.push_back(CIWrapper(info));
  m_cmb_ctrls.append_text(info.get_name(), info.get_number());
  return true;
}


bool TrackDialog::remove_controller(long number) {
  for (unsigned i = 0; i < m_ctrls.size(); ++i) {
    if (number == m_ctrls[i].ci.get_number()) {
      m_ctrls[i].deleted = true;
      m_cmb_ctrls.remove_id(number);
      return true;
    }
  }
  return false;
}


void TrackDialog::add_controller_clicked() {
  m_cdlg.reset();
  m_cdlg.refocus();
  m_cdlg.show_all();
  while (m_cdlg.run() == RESPONSE_OK) {
    const ControllerInfo& info = m_cdlg.get_info();
    if (!is_pbend(info.get_number()) && !is_cc(info.get_number())) {
      cerr<<"Invalid controller!"<<endl;
    }
    else if (!add_controller(info)) {
      cerr<<"Could not add controller!"<<endl;
    }
    else {
      m_cmb_ctrls.set_active_id(info.get_number());
      break;
    }
  }
  m_cdlg.hide();
}


void TrackDialog::remove_controller_clicked() {
  if (m_cmb_ctrls.get_active_id() == -1) {
    cerr<<"No controller selected!"<<endl;
    return;
  }
  
  unsigned i;
  for (i = 0; i < m_ctrls.size(); ++i) {
    if (m_ctrls[i].ci.get_number() == m_cmb_ctrls.get_active_id())
      break;
  }
  if (i == m_ctrls.size()) {
    assert(!"Non-existing controller selected!");
    return;
  }

  remove_controller(m_cmb_ctrls.get_active_id());
  
}


void TrackDialog::modify_controller_clicked() {
  if (m_cmb_ctrls.get_active_id() == -1) {
    cerr<<"No controller selected!"<<endl;
    return;
  }
  
  unsigned i;
  for (i = 0; i < m_ctrls.size(); ++i) {
    if (m_ctrls[i].ci.get_number() == m_cmb_ctrls.get_active_id())
      break;
  }
  if (i == m_ctrls.size()) {
    assert(!"Non-existing controller selected!");
    return;
  }
  
  m_cdlg.set_info(m_ctrls[i].ci);
  m_cdlg.refocus();
  m_cdlg.show_all();
  while (m_cdlg.run() == RESPONSE_OK) {
    const ControllerInfo& info = m_cdlg.get_info();
    if (!is_pbend(info.get_number()) && !is_cc(info.get_number())) {
      cerr<<"Invalid controller!"<<endl;
    }
    else {
      unsigned j;
      for (j = 0; j < m_ctrls.size(); ++j) {
	if (j == i)
	  continue;
	if (m_ctrls[j].ci.get_number() == info.get_number())
	  break;
      }
      if (j < m_ctrls.size())
	cerr<<"That controller number is already used!"<<endl;
      else
	break;
    }
  }
  
  m_cmb_ctrls.remove_id(m_ctrls[i].ci.get_number());
  m_ctrls[i].ci = m_cdlg.get_info();
  m_cmb_ctrls.append_text(m_ctrls[i].ci.get_name(), m_ctrls[i].ci.get_number());
  m_cmb_ctrls.set_active_id(m_ctrls[i].ci.get_number());

  m_cdlg.hide();
}


bool TrackDialog::add_key(const KeyInfo& info) {
  cerr<<"Adding key "<<info.get_number()<<" ("<<info.get_name()<<")"<<endl;
  for (unsigned i = 0; i < m_keys.size(); ++i) {
    if (info.get_number() == m_ctrls[i].ci.get_number())
      return false;
  }
  m_keys.push_back(KIWrapper(info));
  m_cmb_keys.append_text(info.get_name(), info.get_number());
  return true;
}


bool TrackDialog::remove_key(long number) {
  for (unsigned i = 0; i < m_keys.size(); ++i) {
    if (number == m_keys[i].ki.get_number()) {
      m_keys[i].deleted = true;
      m_cmb_keys.remove_id(number);
      return true;
    }
  }
  return false;
}


void TrackDialog::add_key_clicked() {
  m_kdlg.reset();
  m_kdlg.refocus();
  m_kdlg.show_all();
  while (m_kdlg.run() == RESPONSE_OK) {
    const KeyInfo& info = m_kdlg.get_info();
    if (info.get_number() > 127)
      cerr<<"Invalid key!"<<endl;
    else if (!add_key(info))
      cerr<<"Could not add key!"<<endl;
    else {
      m_cmb_keys.set_active_id(info.get_number());
      break;
    }
  }
  m_kdlg.hide();
}


void TrackDialog::remove_key_clicked() {
  
}


void TrackDialog::modify_key_clicked() {

}


