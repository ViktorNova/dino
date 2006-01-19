#ifndef XMLSERIALISABLE_HPP
#define XMLSERIALISABLE_HPP

#include <libxml++/libxml++.h>


/** This is an ABC for all classes that should be able to read and write
    themselves to an XML element. */
class XMLSerialisable {
public:
  
  virtual ~XMLSerialisable() { }
  
  virtual bool fill_xml_node(xmlpp::Element* elt) const = 0;
  virtual bool parse_xml_node(const xmlpp::Element* elt) = 0;
  
protected:

};


#endif
