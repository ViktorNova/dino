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

#ifndef TRACKDIALOG_HPP
#define TRACKDIALOG_HPP

#include <gtkmm.h>
#include <libglademm.h>

#include "singletextcombo.hpp"


namespace Dino {
  class Sequencer;
}

class TrackDialog : public Gtk::Dialog {
public:
  
  TrackDialog(BaseObjectType* obj, const Glib::RefPtr<Gnome::Glade::Xml>& xml);
  
  std::string get_name() const;
  std::string get_port() const;
  int get_channel() const;
  
  void set_name(const std::string& name);
  void set_channel(int channel);
  void update_ports(const Dino::Sequencer* seq = 0);

  void refocus();
  
protected:

  Gtk::Entry* m_ent_name;
  SingleTextCombo m_cmb_port;
  Gtk::SpinButton* m_sbn_channel;

};


#endif
