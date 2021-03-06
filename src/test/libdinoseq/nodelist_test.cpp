/*****************************************************************************
    libdinoseq_test - unit test module for libdinoseq
    Copyright (C) 2009  Lars Luthman <mail@larsluthman.net>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include <algorithm>
#include <iterator>

#include "dtest.hpp"
#include "nodelist.hpp"


using namespace Dino;


/* We can't really test the atomicity in any reasonable way, so we do some
   read and write tests. */

namespace NodeListTest {


  void dtest_constructor() {
    DTEST_NOTHROW(NodeList<int> nl);
  }

  void dtest_insert_remove_read() {
    typedef NodeList<int>::NodeBase NodeBase;
    typedef NodeList<int>::Node Node;
    
    NodeList<int> nl;
    NodeList<int>::NodeBase* end = nl.end_marker();
    
    DTEST_TRUE(nl.first_node() == nl.end_marker());
    
    nl.insert(end, new Node(1));
    
    DTEST_TRUE(nl.first_node() != nl.end_marker());
    
    nl.insert(end, new Node(2));
    nl.insert(nl.first_node(), new Node(0));
    
    NodeBase* nb = nl.first_node();
    
    DTEST_TRUE(static_cast<Node*>(nb)->m_data == 0);
    
    nb = static_cast<Node*>(nb)->m_next.get();
    
    DTEST_TRUE(static_cast<Node*>(nb)->m_data == 1);
    
    nb = static_cast<Node*>(nb)->m_next.get();
    
    DTEST_TRUE(static_cast<Node*>(nb)->m_data == 2);
    
    NodeList<int> const& nlc = nl;
    NodeBase const* nbc = nlc.first_node();
    
    DTEST_TRUE(static_cast<Node const*>(nbc)->m_data == 0);
    
    nbc = static_cast<Node const*>(nbc)->m_next.get();
    
    DTEST_TRUE(static_cast<Node const*>(nbc)->m_data == 1);
    
    nbc = static_cast<Node const*>(nbc)->m_next.get();
    
    DTEST_TRUE(static_cast<Node const*>(nbc)->m_data == 2);
    
    Node* n = static_cast<Node*>(nl.first_node());
    nl.remove(n);
    delete n;
    n = static_cast<Node*>(nl.first_node());
    nl.remove(n);
    delete n;
    n = static_cast<Node*>(nl.first_node());
    nl.remove(n);
    delete n;
    
    DTEST_TRUE(nl.first_node() == nl.end_marker());
  }


}
