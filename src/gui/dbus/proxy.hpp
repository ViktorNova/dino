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

#ifndef PROXY_HPP

#include <string>
#include <vector>

#include <sigc++/sigc++.h>

#include "argument.hpp"


namespace DBus {

  
  class Connection;
  
  
  class Proxy {
  public:
    
    typedef sigc::slot<bool, int, Argument*> SignalHandler;
    typedef sigc::slot<bool, int, Argument*> ReturnHandler;

    Proxy(Connection& conn, const std::string& name, 
	  const std::string& path, const std::string& interface);
    
    void connect(const std::string& member, SignalHandler& handler);
    
    std::vector<Argument> call(const std::string& member, 
			       const Argument& a1 = Argument(),
			       const Argument& a2 = Argument(), 
			       const Argument& a3 = Argument(),
			       const Argument& a4 = Argument(), 
			       const Argument& a5 = Argument());

    void call_async(const std::string& member,
		    ReturnHandler& handler,
		    const Argument& a1 = Argument(),
		    const Argument& a2 = Argument(), 
		    const Argument& a3 = Argument(),
		    const Argument& a4 = Argument(), 
		    const Argument& a5 = Argument());
    
  protected:
    
    Connection& m_conn;
    std::string m_name;
    std::string m_path;
    std::string m_interface;

  };


}


#endif
