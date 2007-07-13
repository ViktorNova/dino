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

#ifndef EVILSCROLLEDWINDOW_HPP
#define EVILSCROLLEDWINDOW_HPP

#include <gtkmm.h>


class EvilScrolledWindow : public Gtk::ScrolledWindow {
public:
  
  EvilScrolledWindow(bool horizontal_evilness = true, 
		     bool vertical_evilness = true) 
    : ScrolledWindow(),
      m_hor_evil(horizontal_evilness), m_ver_evil(vertical_evilness) {
    set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_NEVER);
  }
  
  virtual void on_size_request(Gtk::Requisition* req) {
    Gtk::ScrolledWindow::on_size_request(req);
    if (m_hor_evil)
      req->width = 0;
    if (m_ver_evil)
      req->height = 0;
  }

private:
  bool m_hor_evil;
  bool m_ver_evil;
};


#endif

