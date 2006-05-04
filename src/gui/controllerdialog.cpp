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

#include <iomanip>
#include <sstream>

#include "controller_numbers.hpp"
#include "controllerdialog.hpp"

using namespace Glib;
using namespace Gtk;
using namespace Gnome::Glade;
using namespace std;


ControllerDialog::ControllerDialog(BaseObjectType* obj, 
				   const RefPtr<Xml>& xml) 
  : Dialog(obj) {
  m_ent_name = dynamic_cast<Entry*>(xml->get_widget("dlgcont_ent_name"));
  Box* box = dynamic_cast<Box*>(xml->get_widget("dlgcont_box_controller"));
  box->pack_start(m_cmb_controller);

  m_cmb_controller.append_text("Pitchbend", Dino::make_pbend());

  ostringstream oss;
  for (unsigned i = 0; i < 128; ++i) {
    oss<<"CC"<<setw(3)<<setfill('0')<<i<<": "<<m_cc_desc[i];
    m_cmb_controller.append_text(oss.str(), Dino::make_cc(i));
    oss.str("");
  }
  
  m_cmb_controller.signal_changed().
    connect(mem_fun(*this, &ControllerDialog::update_entry));
  m_ent_name->select_region(0, -1);
  m_cmb_controller.set_active_id(1);
  update_entry();
}


std::string ControllerDialog::get_name() const {
  return m_ent_name->get_text();
}


long ControllerDialog::get_controller() const {
  return m_cmb_controller.get_active_id();
}
  

void ControllerDialog::set_name(const std::string& name) {
  m_ent_name->set_text(name);
}


void ControllerDialog::set_controller(long controller) {
  m_cmb_controller.set_active_id(controller);
}


void ControllerDialog::refocus() {
  m_ent_name->select_region(0, -1);
  m_ent_name->grab_focus();
}


void ControllerDialog::update_entry() {
  int a, b;
  m_ent_name->get_selection_bounds(a, b);
  if (a == 0 && b == (int)m_ent_name->get_text().size()) {
    if (Dino::is_cc(m_cmb_controller.get_active_id()))
      m_ent_name->
	set_text(m_cc_desc[Dino::cc_number(m_cmb_controller.get_active_id())]);
    else if (Dino::is_pbend(m_cmb_controller.get_active_id()))
      m_ent_name->set_text("Pitchbend");
    m_ent_name->select_region(0, -1);
  }
}


string ControllerDialog::m_cc_desc[] = { "Bank select MSB",
					 "Modulation", 
					 "Breath controller",
					 "Undefined",
					 "Foot controller",
					 "Portamento time",
					 "Data entry MSB",
					 "Channel volume",
					 "Balance", 
					 "Undefined",
					 "Pan",
					 "Expression",
					 "Effect control 1",
					 "Effect control 2",
					 "Undefined",
					 "Undefined",
					 "General purpose controller 1",
					 "General purpose controller 2",
					 "General purpose controller 3",
					 "General purpose controller 4",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Bank select LSB",
					 "Modulation LSB",
					 "Breath controller LSB",
					 "Undefined 03 LSB",
					 "Foot controller LSB",
					 "Portamento time LSB",
					 "Data entry LSB",
					 "Channel volume LSB",
					 "Balance LSB",
					 "Undefined 09 LSB",
					 "Pan LSB",
					 "Expression LSB",
					 "Effect control 1 LSB",
					 "Effect control 2 LSB",
					 "Undefined 14 LSB",
					 "Undefined 15 LSB",
					 "General purpose controller 1 LSB",
					 "General purpose controller 2 LSB",
					 "General purpose controller 3 LSB",
					 "General purpose controller 4 LSB",
					 "Undefined 20 LSB",
					 "Undefined 21 LSB",
					 "Undefined 22 LSB",
					 "Undefined 23 LSB",
					 "Undefined 24 LSB",
					 "Undefined 25 LSB",
					 "Undefined 26 LSB",
					 "Undefined 27 LSB",
					 "Undefined 28 LSB",
					 "Undefined 29 LSB",
					 "Undefined 30 LSB",
					 "Undefined 31 LSB",
					 "Damper pedal on/off",
					 "Portamento on/off",
					 "Sustenuto on/off",
					 "Soft pedal on/off",
					 "Legato on/off",
					 "Hold 2 on/off",
					 "Sound controller 1 - Variation",
					 "Sound controller 2 - Timbre",
					 "Sound controller 3 - Release time",
					 "Sound controller 4 - Attack time",
					 "Sound controller 5 - Brightness",
					 "Sound controller 6 - Decay time",
					 "Sound controller 7 - Vibrato rate",
					 "Sound controller 8 - Vibrato depth",
					 "Sound controller 9 - Vibrato delay",
					 "Sound controller 10",
					 "General purpose controller 5",
					 "General purpose controller 6",
					 "General purpose controller 7",
					 "General purpose controller 8",
					 "Portamento control",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Effect depth 1 - Reverb",
					 "Effect depth 2 - Tremolo",
					 "Effect depth 3 - Chorus",
					 "Effect depth 4 - Celeste",
					 "Effect depth 5 - Phaser",
					 "Data increment",
					 "Data decrement",
					 "NRPN LSB",
					 "NRPN MSB",
					 "RPN LSB",
					 "RPN MSB",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "Undefined",
					 "All sound off",
					 "Reset all controllers",
					 "Local control on/off",
					 "All notes off",
					 "Omni mode off",
					 "Omni mode on",
					 "Monophonic operation",
					 "Polyphonic operation" };

				  
