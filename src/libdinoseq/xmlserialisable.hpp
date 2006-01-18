#ifndef XMLSERIALISABLE_HPP
#define XMLSERIALISABLE_HPP

#include <libxml++/libxml++.h>


class XMLSerialisable {
public:
  
  virtual ~XMLSerialisable() { }
  
  virtual bool fill_xml_node(xmlpp::Element* elt) const = 0;
  virtual bool parse_xml_node(const xmlpp::Element* elt) = 0;
  
protected:

};


#endif
