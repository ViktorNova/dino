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

#ifndef XMLSERIALISABLE_HPP
#define XMLSERIALISABLE_HPP

#include <libxml++/libxml++.h>


namespace Dino {
 
  
  /** This is an ABC for all classes that should be able to read and write
      themselves to an XML element. */
  class XMLSerialisable {
  public:
    
    virtual ~XMLSerialisable() { }
    
    virtual bool fill_xml_node(xmlpp::Element* elt) const = 0;
    virtual bool parse_xml_node(const xmlpp::Element* elt) = 0;
    
  protected:
    
  };
  
}


#endif
