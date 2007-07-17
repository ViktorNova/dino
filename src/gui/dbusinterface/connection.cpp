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

#include <cstring>
#include <iostream>

#include "connection.hpp"
#include "object.hpp"


using namespace std;


namespace DBus {
  
  
  Connection::Connection(const std::string& name)
    : m_conn(0),
      m_error(0) {
    if (!(m_conn = dbus_bus_get_private(DBUS_BUS_SESSION, m_error)))
      return;
    dbus_bus_request_name(m_conn, name.c_str(), 0, m_error);
    dbus_connection_add_filter(m_conn, &Connection::introspect_filter, this, 0);
    generate_xml(&m_root);
  }
  
  
  Connection::~Connection() {
    // XXX delete objects and treenodes here
    if (m_conn) {
      dbus_connection_close(m_conn);
      dbus_connection_unref(m_conn);
    }
  }
    

  const DBusError* Connection::get_error() const {
    return m_error;
  }
  
  
  bool Connection::register_object(const std::string& path, Object* obj) {
    if (path.size() == 0 || path[0] != '/')
      return false;
    DBusObjectPathVTable* vtable = new DBusObjectPathVTable;
    vtable->unregister_function = 0;
    vtable->message_function = &Connection::op_message_function;
    if (dbus_connection_register_object_path(m_conn, path.c_str(),
					     vtable, obj) != TRUE) {
      delete vtable;
      return false;
    }
    cerr<<"Registered object \""<<path<<"\""<<endl;
    TreeNode* node = &m_root;
    int s = 1;
    while (s < path.size()) {
      int e = path.find('/', s);
      e = e < 0 ? path.size() : e;
      string element = path.substr(s, e - s);
      cerr<<element<<endl;
      if (!node->children[element]) {
	node->children[element] = new TreeNode;
	generate_xml(node);
      }
      node = node->children[element];
      s = e + 1;
    }
    if (node->object) {
      delete node->object;
      delete node->vtable;
    }
    node->object = obj;
    node->vtable = vtable;
    generate_xml(node);
    return true;
  }


  bool Connection::run(int msec) {
    return (dbus_connection_read_write_dispatch(m_conn, msec) == TRUE);
  }
  

  void Connection::generate_xml(TreeNode* node) {
    node->xml = "";
    node->xml = node->xml +
      "<!DOCTYPE node PUBLIC "
      "\"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
      "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
      "<node>\n";
    if (node->object) {
      const Object::InterfaceMap& ifs = node->object->get_interfaces();
      Object::InterfaceMap::const_iterator iter;
      for (iter = ifs.begin(); iter != ifs.end(); ++iter) {
	node->xml = node->xml + "  <interface name=\"" + iter->first + "\">\n";
	const map<string, string>& methods = iter->second;
	map<string, string>::const_iterator iter2;
	for (iter2 = methods.begin(); iter2 != methods.end(); ++iter2) {
	  node->xml = node->xml + 
	    "<method name=\"" + iter2->first + "\"/>\n";
	}
	node->xml += "  </interface>\n";
      }
    }
    
    map<string, TreeNode*>::iterator iter;
    for (iter = node->children.begin(); iter != node->children.end(); ++iter)
      node->xml = node->xml + "  <node name=\"" + iter->first + "\"/>";
    node->xml += "</node>\n";
  }


  DBusHandlerResult Connection::introspect_filter(DBusConnection* conn,
						  DBusMessage* msg,
						  void* user_data) {
    if (!dbus_message_is_method_call(msg, "org.freedesktop.DBus.Introspectable",
				     "Introspect"))
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    
    Connection* me = static_cast<Connection*>(user_data);
    TreeNode* node = &me->m_root;
    string path = dbus_message_get_path(msg);

    cerr<<"Introspect call received for "<<path<<endl;
    
    int s = 1;
    while (s < path.size()) {
      int e = path.find('/', s);
      e = e < 0 ? path.size() : e;
      string element = path.substr(s, e - 1);
      map<string, TreeNode*>::iterator iter = node->children.find(element);
      if (iter == node->children.end()) {
	node = 0;
	break;
      }
      node = iter->second;
      s = e + 1;
    }

    DBusMessage* ret;
    if (node) {
      ret = dbus_message_new_method_return(msg);
      const char* xml = node->xml.c_str();
      dbus_message_append_args(ret, DBUS_TYPE_STRING, &xml, DBUS_TYPE_INVALID);
    }
    else {
      ret = dbus_message_new_error(msg, DBUS_ERROR_FAILED, "No such object");
    }
    dbus_connection_send(conn, ret, 0);
    return DBUS_HANDLER_RESULT_HANDLED;
  }
  
    
  void Connection::op_unregister_function(DBusConnection* conn, 
					  void* user_data) {

  }


  DBusHandlerResult Connection::op_message_function(DBusConnection* conn, 
						    DBusMessage* msg,
						    void* user_data) {
    Object* obj = static_cast<Object*>(user_data);
    return obj->message_function(conn, msg);
  }
  
  
}

