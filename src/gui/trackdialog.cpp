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

#include <cassert>

#include "sequencer.hpp"
#include "trackdialog.hpp"
#include "controller_numbers.hpp"


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
  Table* table = manage(new Table(6, 2));
  table->attach(*manage(new Label("Track name:")), 0, 1, 0, 1);
  table->attach(*manage(new Label("MIDI port:")), 0, 1, 1, 2);
  table->attach(*manage(new Label("MIDI channel:")), 0, 1, 2, 3);
  table->attach(*manage(new HSeparator), 0, 2, 3, 4);
  table->attach(*manage(new Label("Controllers:")), 0, 1, 4, 5);
  table->attach(*m_ent_name, 1, 2, 0, 1);
  table->attach(m_cmb_port, 1, 2, 1, 2);
  table->attach(*m_sbn_channel, 1, 2, 2, 3);
  table->attach(m_cmb_ctrls, 1, 2, 4, 5);
  HBox* hbox = manage(new HBox(false, 3));
  Button* add_ctrl_btn = manage(new Button("Add"));
  hbox->pack_start(*add_ctrl_btn);
  Button* remove_ctrl_btn = manage(new Button("Remove"));
  hbox->pack_start(*remove_ctrl_btn);
  Button* modify_ctrl_btn = manage(new Button("Modify"));
  hbox->pack_start(*modify_ctrl_btn);
  table->attach(*hbox, 1, 2, 5, 6);
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
  
  update_ports();
}


string TrackDialog::get_name() const {
  return m_ent_name->get_text();
}


string TrackDialog::get_port() const {
  return m_cmb_port.get_active_text();
}


int TrackDialog::get_channel() const {
  return m_sbn_channel->get_value_as_int();
}


const vector<ControllerInfo*>& TrackDialog::get_controllers() const {
  return m_ctrls;
}


void TrackDialog::set_name(const string& name) {
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
    vector<InstrumentInfo> info = seq->get_instruments();
    for (size_t i = 0; i < info.size(); ++i)
      m_cmb_port.append_text(info[i].get_name(), i);
  }
}


void TrackDialog::set_controllers(const vector<ControllerInfo*>& ctrls) {
  for (unsigned i = 0; i < m_ctrls.size(); ++i)
    delete m_ctrls[i];
  m_ctrls.clear();
  m_cmb_ctrls.clear();
  for (unsigned i = 0; i < ctrls.size(); ++i) {
    m_ctrls.push_back(new ControllerInfo(*ctrls[i]));
    m_cmb_ctrls.append_text(m_ctrls[i]->get_name(), m_ctrls[i]->get_number());
  }
}


void TrackDialog::refocus() {
  m_ent_name->select_region(0, -1);
  m_ent_name->grab_focus();
}


bool TrackDialog::add_controller(const ControllerInfo& info) {
  cerr<<"Adding "<<info.get_number()<<" ("<<info.get_name()<<")"<<endl;
  for (unsigned i = 0; i < m_ctrls.size(); ++i) {
    if (info.get_number() == m_ctrls[i]->get_number())
      return false;
  }
  m_ctrls.push_back(new ControllerInfo(info));
  m_cmb_ctrls.append_text(info.get_name(), info.get_number());
  return true;
}


bool TrackDialog::remove_controller(long number) {
  for (unsigned i = 0; i < m_ctrls.size(); ++i) {
    if (number == m_ctrls[i]->get_number()) {
      delete m_ctrls[i];
      m_ctrls[i] = 0;
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
    if (m_ctrls[i]->get_number() == m_cmb_ctrls.get_active_id())
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
    if (m_ctrls[i]->get_number() == m_cmb_ctrls.get_active_id())
      break;
  }
  if (i == m_ctrls.size()) {
    assert(!"Non-existing controller selected!");
    return;
  }
  
  m_cdlg.set_info(*m_ctrls[i]);
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
	if (m_ctrls[j]->get_number() == info.get_number())
	  break;
      }
      if (j < m_ctrls.size())
	cerr<<"That controller number is already used!"<<endl;
      else
	break;
    }
  }
  
  m_cmb_ctrls.remove_id(m_ctrls[i]->get_number());
  m_ctrls[i] = new ControllerInfo(m_cdlg.get_info());
  m_cmb_ctrls.append_text(m_ctrls[i]->get_name(), m_ctrls[i]->get_number());
  m_cmb_ctrls.set_active_id(m_ctrls[i]->get_number());

  m_cdlg.hide();
}


