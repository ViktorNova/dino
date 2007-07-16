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

#ifndef CONNECTION_HPP

#include <string>
#include <map>

#include <dbus/dbus.h>


/** C++ wrapper for the low-level D-Bus API. */
namespace DBus {
  
  
  class Object;
  
  
  /** An abstraction for a D-Bus connection. It is not a complete 
      implementation, it just has the functions needed to expose objects 
      with methods and signals on the session bus. */
  class Connection {
  public:
    
    /** Create a new connection to the session bus and request a known name. */
    Connection(const std::string& name);
    
    /** Disconnect from the session bus and free all resources used by 
	registered objects. */
    ~Connection();
    
    /** Return the error for the latest D-Bus API call. */
    const DBusError* get_error() const;
    
    /** Register an object at a path. This Connection object will take over
	ownership of the Object pointer, and any previous object at the
	same path will be deleted. Any methods or signals added to the object
	after its registration will be ignored. */
    bool register_object(const std::string& path, Object* obj);
    
    /** Unregister the object at the given path and free any resources used by
	it. */
    bool unregister_object(const std::string& path);
    
    /** Read messages from the bus, dispatch registered method calls, and
	send signals and replies. */
    bool run(int msec);
    
  protected:
    
    /** This structure is used internally in the object tree. */
    struct TreeNode {
      TreeNode() : object(0), vtable(0) { }
      Object* object;
      DBusObjectPathVTable* vtable;
      std::map<std::string, TreeNode*> children;
      std::string xml;
    };
    
    /** This function generates the introspection XML snippet for a node
	in the object tree. */
    void generate_xml(TreeNode* node);
    
    /** A message filter that catches any "Introspect" calls. */
    static DBusHandlerResult introspect_filter(DBusConnection* conn,
					       DBusMessage* msg,
					       void* user_data);
    
    /** Called when an object is unregistered. */
    static void op_unregister_function(DBusConnection* conn, void* user_data);
    
    /** Called when a message is received for an object. */
    static DBusHandlerResult op_message_function(DBusConnection* conn, 
						 DBusMessage* msg,
						 void* user_data);
    
    /** The D-Bus connection. */
    DBusConnection* m_conn;
    /** A place to store any D-Bus API errors. */
    DBusError* m_error;
    /** The root of the object tree, with path "/". */
    TreeNode m_root;

  };


}


#endif
