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

#include "sequencer.hpp"
#include "trackdialog.hpp"


using namespace Gtk;
using namespace Gnome::Glade;
using namespace Glib;
using namespace std;
using namespace Dino;


TrackDialog::TrackDialog(BaseObjectType* obj, const RefPtr<Xml>& xml)
  : Dialog(obj) {
  
  m_ent_name = dynamic_cast<Entry*>(xml->get_widget("dlgtrack_ent_name"));
  m_sbn_channel = 
    dynamic_cast<SpinButton*>(xml->get_widget("dlgtrack_sbn_channel"));
  dynamic_cast<VBox*>(xml->get_widget("dlgtrack_vbx_port"))->
    pack_start(m_cmb_port);
  
  manage(m_ent_name);
  manage(m_sbn_channel);
  
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
