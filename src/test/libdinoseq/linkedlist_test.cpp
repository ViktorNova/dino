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

#include <boost/test/unit_test.hpp>

#include "linkedlist.hpp"


using namespace Dino;


/* We can't really test the atomicity in any reasonable way, so we do some
   read and write tests. */

BOOST_AUTO_TEST_SUITE(LinkedListTest)


BOOST_AUTO_TEST_CASE(constructor) {
  BOOST_CHECK_NO_THROW(LinkedList<int> ll);
}

BOOST_AUTO_TEST_CASE(insert_erase_size) {
  LinkedList<int> ll;
  auto end = ll.end();
  
  ll.insert(end, 1);
  
  BOOST_CHECK_EQUAL(ll.get_size(), 1);

  ll.insert(end, 2);
  
  BOOST_CHECK_EQUAL(ll.get_size(), 2);

  ll.insert(end, 3);
  
  BOOST_CHECK_EQUAL(ll.get_size(), 3);
  
  ll.erase(ll.begin());

  BOOST_CHECK_EQUAL(ll.get_size(), 2);

  ll.erase(ll.begin());
  
  BOOST_CHECK_EQUAL(ll.get_size(), 1);
  
  ll.erase(ll.begin());
  
  BOOST_CHECK_EQUAL(ll.get_size(), 0);
  
}


BOOST_AUTO_TEST_CASE(begin_end) {
  LinkedList<int> ll;
  
  LinkedList<int>::Iterator begin = ll.begin();
  
  BOOST_CHECK(ll.begin() == ll.end());

  ll.insert(ll.end(), 1);
  
  BOOST_CHECK(begin != ll.begin());
  
  BOOST_CHECK(ll.begin() != ll.end());
  
  ll.erase(ll.begin());
  
  BOOST_CHECK(ll.begin() == ll.end());
  
  BOOST_CHECK(ll.begin() == begin);
  
}


BOOST_AUTO_TEST_CASE(begin_end_const) {
  LinkedList<int> ll;
  LinkedList<int> const& ll_const = ll;
  
  LinkedList<int>::ConstIterator begin = ll_const.begin();
  
  BOOST_CHECK(begin == ll_const.end());

  ll.insert(ll.end(), 1);
  
  BOOST_CHECK(begin != ll_const.begin());
  
  BOOST_CHECK(ll_const.begin() != ll_const.end());
  
  ll.erase(ll.begin());
  
  BOOST_CHECK(ll_const.begin() == ll_const.end());
  
  BOOST_CHECK(ll_const.begin() == begin);
  
}


BOOST_AUTO_TEST_CASE(reader_begin_reader_end) {
  LinkedList<int> ll;
  
  LinkedList<int>::ReaderIterator begin = ll.reader_begin();
  
  BOOST_CHECK(begin == ll.reader_end());

  ll.insert(ll.end(), 1);
  
  BOOST_CHECK(begin != ll.reader_begin());
  
  BOOST_CHECK(ll.reader_begin() != ll.reader_end());
  
  ll.erase(ll.begin());
  
  BOOST_CHECK(ll.reader_begin() == ll.reader_end());
  
  BOOST_CHECK(ll.reader_begin() == begin);
  
}


BOOST_AUTO_TEST_CASE(queued_deletion) {
  LinkedList<int> ll;
  ll.insert(ll.end(), 1);
  ll.insert(ll.end(), 2);
  ll.insert(ll.end(), 3);
  ll.erase(ll.begin());
  ll.erase(ll.begin());
  ll.erase(ll.begin());
  
  BOOST_CHECK(ll.delete_erased_nodes() == 0);
  
  ll.reader_holds_no_iterator();
  
  BOOST_CHECK(ll.delete_erased_nodes() == 3);

  BOOST_CHECK(ll.delete_erased_nodes() == 0);
}


BOOST_AUTO_TEST_CASE(Iterator) {
  LinkedList<int> ll;
  LinkedList<int> const& ll_const = ll;
  ll.insert(ll.end(), 1);
  ll.insert(ll.end(), 2);
  ll.insert(ll.end(), 3);
  
  LinkedList<int>::Iterator iter = ll.begin();
  
  BOOST_CHECK(iter == ll_const.begin());
  
  BOOST_CHECK(*(iter.operator->()) == 1);
  
  BOOST_CHECK(*iter == 1);
  
  ++iter;
  
  BOOST_CHECK(*iter == 2);
  
  BOOST_CHECK(iter != ll.begin());
  
  --iter;
  
  BOOST_CHECK(*iter == 1);
  
  LinkedList<int>::Iterator iter2 = iter++;
  
  BOOST_CHECK(*iter == 2);
  
  BOOST_CHECK(*iter2 == 1);
  
  iter2 = iter--;
  
  BOOST_CHECK(*iter == 1);
  
  BOOST_CHECK(*iter2 == 2);
  
  LinkedList<int>::ConstIterator c_iter;
  
  BOOST_CHECK_NO_THROW(c_iter = iter);
  
  BOOST_CHECK((*iter2 = 5) == 5);
  
  typedef std::reverse_iterator<LinkedList<int>::Iterator> RevIter;
  std::ostringstream oss;
  std::copy(RevIter(ll.end()), RevIter(ll.begin()), 
	    std::ostream_iterator<int>(oss));
  
  BOOST_CHECK(oss.str() == "351");
  
  ll.erase(++ll.begin());
  oss.str("");
  std::copy(RevIter(ll.end()), RevIter(ll.begin()), 
	    std::ostream_iterator<int>(oss));
  
  BOOST_CHECK(oss.str() == "31");
}


BOOST_AUTO_TEST_CASE(ConstIterator) {
  LinkedList<int> ll;
  LinkedList<int> const& ll_const = ll;
  ll.insert(ll.end(), 1);
  ll.insert(ll.end(), 2);
  ll.insert(ll.end(), 3);
  
  LinkedList<int>::ConstIterator iter = ll_const.begin();
  
  BOOST_CHECK(iter == ll_const.begin());
  
  BOOST_CHECK(*(iter.operator->()) == 1);
  
  BOOST_CHECK(*iter == 1);
  
  ++iter;
  
  BOOST_CHECK(*iter == 2);
  
  BOOST_CHECK(iter != ll_const.begin());
  
  --iter;
  
  BOOST_CHECK(*iter == 1);
  
  LinkedList<int>::ConstIterator iter2 = iter++;
  
  BOOST_CHECK(*iter == 2);
  
  BOOST_CHECK(*iter2 == 1);
  
  iter2 = iter--;
  
  BOOST_CHECK(*iter == 1);
  
  BOOST_CHECK(*iter2 == 2);
  
  typedef std::reverse_iterator<LinkedList<int>::ConstIterator> RevIter;
  std::ostringstream oss;
  std::copy(RevIter(ll_const.end()), RevIter(ll_const.begin()), 
	    std::ostream_iterator<int>(oss));
  
  BOOST_CHECK(oss.str() == "321");
  
}


BOOST_AUTO_TEST_CASE(ReaderIterator) {
  LinkedList<int> ll;
  ll.insert(ll.end(), 1);
  ll.insert(ll.end(), 2);
  ll.insert(ll.end(), 3);
  
  LinkedList<int>::ReaderIterator iter = ll.reader_begin();
  
  BOOST_CHECK(iter == ll.reader_begin());
  
  BOOST_CHECK(*(iter.operator->()) == 1);
  
  BOOST_CHECK(*iter == 1);
  
  ++iter;
  
  BOOST_CHECK(*iter == 2);
  
  BOOST_CHECK(iter != ll.reader_begin());
  
  LinkedList<int>::ReaderIterator iter2 = iter++;
  
  BOOST_CHECK(*iter == 3);
  
  BOOST_CHECK(*iter2 == 2);
  
  std::ostringstream oss;
  std::copy(ll.reader_begin(), ll.reader_end(), 
	    std::ostream_iterator<int>(oss));
  
  BOOST_CHECK(oss.str() == "123");
  
}


BOOST_AUTO_TEST_SUITE_END()
