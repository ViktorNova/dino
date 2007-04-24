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

#ifndef TRACKDIALOG_HPP
#define TRACKDIALOG_HPP

#include <vector>

#include <gtkmm.h>

#include "controllerdialog.hpp"
#include "controllerinfo.hpp"
#include "singletextcombo.hpp"


namespace Dino {
  class Sequencer;
  class Track;
}


class TrackDialog : public Gtk::Dialog {
public:
  
  TrackDialog();
  
  std::string get_name() const;
  std::string get_port() const;
  int get_channel() const;
  
  void set_name(const std::string& name);
  void set_channel(int channel);
  void update_ports(const Dino::Sequencer* seq = 0);
  void set_controllers(const std::vector<Dino::ControllerInfo*>& ctrls);
  
  void refocus();
  
  void reset();
  void set_track(const Dino::Track& track, Dino::Sequencer& seq);
  void apply_to_track(Dino::Track& track, Dino::Sequencer& seq);
  
protected:
  
  bool add_controller(const Dino::ControllerInfo& info);
  bool remove_controller(long number);
  
  void add_controller_clicked();
  void remove_controller_clicked();
  void modify_controller_clicked();
  
  struct CIWrapper {
    CIWrapper(const Dino::ControllerInfo& _ci) : ci(_ci), deleted(false) { }
    Dino::ControllerInfo ci;
    bool deleted;
  };
  
  Gtk::Entry* m_ent_name;
  SingleTextCombo m_cmb_port;
  Gtk::SpinButton* m_sbn_channel;
  SingleTextCombo m_cmb_ctrls;
  std::vector<CIWrapper> m_ctrls;
  ControllerDialog m_cdlg;
  
};


#endif
