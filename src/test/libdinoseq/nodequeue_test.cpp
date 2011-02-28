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

#include <boost/test/unit_test.hpp>

#include "nodequeue.hpp"


using namespace Dino;


/* We can't really test the atomicity in any reasonable way, so we do some
   read and write tests. */

BOOST_AUTO_TEST_SUITE(NodeQueueTest)


BOOST_AUTO_TEST_CASE(constructor) {
  BOOST_CHECK_NO_THROW(NodeQueue<int> nq);
}

BOOST_AUTO_TEST_CASE(push_pop) {
  NodeQueue<int> nq;
  typedef NodeQueue<int>::Node Node;
  
  nq.push_node(new Node(1));
  nq.push_node(new Node(2));
  nq.push_node(new Node(3));
  
  Node* node;
  
  BOOST_CHECK((node = nq.pop_node()) != 0);
  
  BOOST_CHECK(node->data == 1);
  
  delete node;
  
  BOOST_CHECK((node = nq.pop_node()) != 0);
  
  BOOST_CHECK(node->data == 2);

  delete node;
  
  BOOST_CHECK((node = nq.pop_node()) == 0);
  
  nq.push_node(new Node(4));
  
  BOOST_CHECK((node = nq.pop_node()) != 0);
  
  BOOST_CHECK(node->data == 3);
  
  delete node;
}


BOOST_AUTO_TEST_SUITE_END()
