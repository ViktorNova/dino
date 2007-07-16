#include <iostream>

#include "object.hpp"


using namespace std;


namespace DBus {
  

  Object::Object() {

  }
  
  
  Object::~Object() {
    
  }
  
    
  void Object::add_method(const std::string& interface, 
			  const std::string& method,
			  const std::string& typesig) {
    m_ifs[interface][method] = typesig;
  }
  
  
  void Object::add_signal(const std::string& interface, 
			  const std::string& signal,
			  const std::string& typesig) {
    
  }
  
  
  const Object::InterfaceMap& Object::get_interfaces() const {
    return m_ifs;
  }


  DBusHandlerResult Object::message_function(DBusConnection* conn, 
					     DBusMessage* msg) {
    cerr<<__PRETTY_FUNCTION__<<endl;
  }
  
}
