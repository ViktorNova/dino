#ifndef OBJECT_HPP

#include <string>
#include <map>

#include <dbus/dbus.h>


namespace DBus {
  
  
  /** A class that represents a D-Bus object. It must be registered using
      DBus::Connection::register_object() to be accessible over the bus. */
  class Object {
  public:
    
    /** A convenient typedef. */
    typedef std::map<std::string, std::map<std::string, std::string> > 
    InterfaceMap;
    
    Object();
    ~Object();
    
    /** Add a new method to the object. */
    void add_method(const std::string& interface, const std::string& method,
		    const std::string& typesig);
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
