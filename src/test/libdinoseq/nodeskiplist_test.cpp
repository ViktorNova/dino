/*****************************************************************************
    libdinoseq_test - unit test module for libdinoseq
    Copyright (C) 2009  Lars Luthman <lars.luthman@gmail.com>
    
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

#include <boost/test/unit_test.hpp>

#include "nodeskiplist.hpp"


using namespace Dino;


/* We can't really test the atomicity in any reasonable way, so we do some
   read and write tests. */

BOOST_AUTO_TEST_SUITE(NodeSkipListTest)


BOOST_AUTO_TEST_CASE(constructor) {
  BOOST_CHECK_NO_THROW(NodeSkipList<int> nl);
}


BOOST_AUTO_TEST_CASE(first_node_head_end_marker) {
  NodeSkipList<int> nsl;
  
  BOOST_CHECK(nsl.first_node() != nsl.head_marker());

  BOOST_CHECK(nsl.first_node() == nsl.end_marker());

  BOOST_CHECK(nsl.head_marker() != nsl.end_marker());
  
  BOOST_CHECK(nsl.head_marker()->links[0].next.get() == nsl.first_node());

  BOOST_CHECK(nsl.head_marker()->links[0].next.get() == nsl.end_marker());
}


BOOST_AUTO_TEST_CASE(insert_remove) {
  typedef NodeSkipList<int>::NodeBase NodeBase;
  typedef NodeSkipList<int>::Node Node;
  
  NodeSkipList<int> nl;
  NodeBase* end = nl.end_marker();
  
  BOOST_CHECK(nl.first_node() == nl.end_marker());
  
  BOOST_CHECK(nl.insert(end, new Node(1)));

  BOOST_CHECK(nl.first_node() != nl.end_marker());
  
  BOOST_CHECK(nl.insert(end, new Node(2)));
  
  BOOST_CHECK(nl.insert(nl.first_node(), new Node(0)));
  
  NodeBase* nb = nl.first_node();
  
  BOOST_CHECK(static_cast<Node*>(nb)->data == 0);

  nb = static_cast<Node*>(nb)->links[0].next.get();
  
  BOOST_CHECK(static_cast<Node*>(nb)->data == 1);
  
  nb = static_cast<Node*>(nb)->links[0].next.get();
  
  BOOST_CHECK(static_cast<Node*>(nb)->data == 2);
  
  Node* n = static_cast<Node*>(nl.first_node());
  nl.remove(n);
  delete n;
  
  n = static_cast<Node*>(nl.first_node());
  
  BOOST_CHECK(n->data == 1);
  
  nl.remove(n);
  delete n;
  n = static_cast<Node*>(nl.first_node());

  BOOST_CHECK(n->data == 2);
  
  nl.remove(n);
  delete n;
  
  BOOST_CHECK(nl.first_node() == nl.end_marker());
}


BOOST_AUTO_TEST_CASE(lower_bound) {
  typedef NodeSkipList<int>::NodeBase NodeBase;
  typedef NodeSkipList<int>::Node Node;

  NodeSkipList<int> nsl;
  
  Node* n1 = new Node(1);
  nsl.insert(nsl.end_marker(), n1);
  Node* n2 = new Node(2);
  nsl.insert(nsl.end_marker(), n2);
  nsl.insert(nsl.end_marker(), new Node(2));
  Node* n3 = new Node(3);
  nsl.insert(nsl.end_marker(), n3);
  nsl.insert(nsl.end_marker(), new Node(3));
  nsl.insert(nsl.end_marker(), new Node(3));
  Node* n4 = new Node(4);
  nsl.insert(nsl.end_marker(), n4);
  
  BOOST_CHECK(nsl.lower_bound(0) == n1);
  
  BOOST_CHECK(nsl.lower_bound(1) == n1);
  
  BOOST_CHECK(nsl.lower_bound(2) == n2);
  
  BOOST_CHECK(nsl.lower_bound(3) == n3);

  BOOST_CHECK(nsl.lower_bound(4) == n4);
  
  BOOST_CHECK(nsl.lower_bound(5) == nsl.end_marker());
  
  NodeSkipList<int> const& nslc = nsl;
  
  BOOST_CHECK(nslc.lower_bound(0) == n1);
  
  BOOST_CHECK(nslc.lower_bound(1) == n1);
  
  BOOST_CHECK(nslc.lower_bound(2) == n2);
  
  BOOST_CHECK(nslc.lower_bound(3) == n3);

  BOOST_CHECK(nslc.lower_bound(4) == n4);
  
  BOOST_CHECK(nslc.lower_bound(5) == nslc.end_marker());
}


BOOST_AUTO_TEST_CASE(upper_bound) {
  typedef NodeSkipList<int>::NodeBase NodeBase;
  typedef NodeSkipList<int>::Node Node;

  NodeSkipList<int> nsl;
  
  Node* n1 = new Node(1);
  nsl.insert(nsl.end_marker(), n1);
  Node* n2 = new Node(2);
  nsl.insert(nsl.end_marker(), n2);
  nsl.insert(nsl.end_marker(), new Node(2));
  Node* n3 = new Node(3);
  nsl.insert(nsl.end_marker(), n3);
  nsl.insert(nsl.end_marker(), new Node(3));
  nsl.insert(nsl.end_marker(), new Node(3));
  Node* n4 = new Node(4);
  nsl.insert(nsl.end_marker(), n4);
  
  BOOST_CHECK(nsl.upper_bound(0) == n1);
  
  BOOST_CHECK(nsl.upper_bound(1) == n2);
  
  BOOST_CHECK(nsl.upper_bound(2) == n3);
  
  BOOST_CHECK(nsl.upper_bound(3) == n4);

  BOOST_CHECK(nsl.upper_bound(4) == nsl.end_marker());
  
  NodeSkipList<int> const& nslc = nsl;
  
  BOOST_CHECK(nslc.upper_bound(0) == n1);
  
  BOOST_CHECK(nslc.upper_bound(1) == n2);
  
  BOOST_CHECK(nslc.upper_bound(2) == n3);
  
  BOOST_CHECK(nslc.upper_bound(3) == n4);

  BOOST_CHECK(nslc.upper_bound(4) == nslc.end_marker());
}


BOOST_AUTO_TEST_CASE(find_less) {
  typedef NodeSkipList<int>::NodeBase NodeBase;
  typedef NodeSkipList<int>::Node Node;

  NodeSkipList<int> nsl;
  
  Node* n1 = new Node(1);
  nsl.insert(nsl.end_marker(), n1);
  nsl.insert(nsl.end_marker(), new Node(2));
  Node* n2 = new Node(2);
  nsl.insert(nsl.end_marker(), n2);
  nsl.insert(nsl.end_marker(), new Node(3));
  nsl.insert(nsl.end_marker(), new Node(3));
  Node* n3 = new Node(3);
  nsl.insert(nsl.end_marker(), n3);
  Node* n4 = new Node(4);
  nsl.insert(nsl.end_marker(), n4);
  
  BOOST_CHECK(nsl.find_less(0) == nsl.head_marker());
  
  BOOST_CHECK(nsl.find_less(1) == nsl.head_marker());
  
  BOOST_CHECK(nsl.find_less(2) == n1);
  
  BOOST_CHECK(nsl.find_less(3) == n2);

  BOOST_CHECK(nsl.find_less(4) == n3);
  
  BOOST_CHECK(nsl.find_less(5) == n4);
  
  NodeSkipList<int> const& nslc = nsl;

  BOOST_CHECK(nslc.find_less(0) == nslc.head_marker());
  
  BOOST_CHECK(nslc.find_less(1) == nslc.head_marker());
  
  BOOST_CHECK(nslc.find_less(2) == n1);
  
  BOOST_CHECK(nslc.find_less(3) == n2);

  BOOST_CHECK(nslc.find_less(4) == n3);
  
  BOOST_CHECK(nslc.find_less(5) == n4);
}


BOOST_AUTO_TEST_CASE(find_less_or_equal) {
  typedef NodeSkipList<int>::NodeBase NodeBase;
  typedef NodeSkipList<int>::Node Node;

  NodeSkipList<int> nsl;
  
  Node* n1 = new Node(1);
  nsl.insert(nsl.end_marker(), n1);
  nsl.insert(nsl.end_marker(), new Node(2));
  Node* n2 = new Node(2);
  nsl.insert(nsl.end_marker(), n2);
  nsl.insert(nsl.end_marker(), new Node(3));
  nsl.insert(nsl.end_marker(), new Node(3));
  Node* n3 = new Node(3);
  nsl.insert(nsl.end_marker(), n3);
  Node* n4 = new Node(4);
  nsl.insert(nsl.end_marker(), n4);
  
  BOOST_CHECK(nsl.find_less_or_equal(0) == nsl.head_marker());
  
  BOOST_CHECK(nsl.find_less_or_equal(1) == n1);
  
  BOOST_CHECK(nsl.find_less_or_equal(2) == n2);
  
  BOOST_CHECK(nsl.find_less_or_equal(3) == n3);

  BOOST_CHECK(nsl.find_less_or_equal(4) == n4);
  
  BOOST_CHECK(nsl.find_less_or_equal(5) == n4);
  
  NodeSkipList<int> const& nslc = nsl;

  BOOST_CHECK(nslc.find_less_or_equal(0) == nslc.head_marker());
  
  BOOST_CHECK(nslc.find_less_or_equal(1) == n1);
  
  BOOST_CHECK(nslc.find_less_or_equal(2) == n2);
  
  BOOST_CHECK(nslc.find_less_or_equal(3) == n3);

  BOOST_CHECK(nslc.find_less_or_equal(4) == n4);
  
  BOOST_CHECK(nslc.find_less_or_equal(5) == n4);
}



BOOST_AUTO_TEST_SUITE_END()
