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

#ifndef OBJECT_HPP

#include <string>
#include <map>

#include <dbus/dbus.h>
#include <sigc++/sigc++.h>


namespace DBus {
  
  
  class Argument;
  
  
  /** A class that represents a D-Bus object. It must be registered using
      DBus::Connection::register_object() to be accessible over the bus. */
  class Object {
  public:
    
    /** A convenient typedef. */
    typedef sigc::slot<bool, int, Argument*> Method;
    
    /** Another convenient typedef. */
    typedef std::map<std::string, std::map<std::string, std::map<std::string,
								 Method> > > 
    InterfaceMap;
    
    Object();
    ~Object();
    
    /** Add a new method to the object. */
    void add_method(const std::string& interface, const std::string& method,
		    const std::string& typesig, Method handler);
    /** Add a new signal to the object. */
    void add_signal(const std::string& interface, const std::string& signal,
		    const std::string& typesig);
    
    /** Return all the D-Bus interfaces this object implements. */
    const InterfaceMap& get_interfaces() const;
    
    /** This is called by DBus::Connection when a message is received for
	this object. */
    DBusHandlerResult message_function(DBusConnection* conn, DBusMessage* msg);
    
  protected:
    
    InterfaceMap m_ifs;
    
  };


}


#endif
