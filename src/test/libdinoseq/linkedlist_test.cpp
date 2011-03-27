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
#include <sstream>

#include "dtest.hpp"
#include "linkedlist.hpp"


using namespace Dino;


/* We can't really test the atomicity in any reasonable way, so we do some
   read and write tests. */

namespace LinkedListTest {


  void dtest_constructor() {
  DTEST_NOTHROW(LinkedList<int> ll);
}

  void dtest_insert_erase_size() {
  LinkedList<int> ll;
  auto end = ll.end();
  
  ll.insert(end, 1);
  
  DTEST_TRUE(ll.get_size() == 1);

  ll.insert(end, 2);
  
  DTEST_TRUE(ll.get_size() == 2);

  ll.insert(end, 3);
  
  DTEST_TRUE(ll.get_size() == 3);
  
  ll.erase(ll.begin());

  DTEST_TRUE(ll.get_size() == 2);

  ll.erase(ll.begin());
  
  DTEST_TRUE(ll.get_size() == 1);
  
  ll.erase(ll.begin());
  
  DTEST_TRUE(ll.get_size() == 0);
  
}


  void dtest_begin_end() {
  LinkedList<int> ll;
  
  LinkedList<int>::Iterator begin = ll.begin();
  
  DTEST_TRUE(ll.begin() == ll.end());

  ll.insert(ll.end(), 1);
  
  DTEST_TRUE(begin != ll.begin());
  
  DTEST_TRUE(ll.begin() != ll.end());
  
  ll.erase(ll.begin());
  
  DTEST_TRUE(ll.begin() == ll.end());
  
  DTEST_TRUE(ll.begin() == begin);
  
}


  void dtest_begin_end_const() {
  LinkedList<int> ll;
  LinkedList<int> const& ll_const = ll;
  
  LinkedList<int>::ConstIterator begin = ll_const.begin();
  
  DTEST_TRUE(begin == ll_const.end());

  ll.insert(ll.end(), 1);
  
  DTEST_TRUE(begin != ll_const.begin());
  
  DTEST_TRUE(ll_const.begin() != ll_const.end());
  
  ll.erase(ll.begin());
  
  DTEST_TRUE(ll_const.begin() == ll_const.end());
  
  DTEST_TRUE(ll_const.begin() == begin);
  
}


  void dtest_reader_begin_reader_end() {
  LinkedList<int> ll;
  
  LinkedList<int>::ReaderIterator begin = ll.reader_begin();
  
  DTEST_TRUE(begin == ll.reader_end());

  ll.insert(ll.end(), 1);
  
  DTEST_TRUE(begin != ll.reader_begin());
  
  DTEST_TRUE(ll.reader_begin() != ll.reader_end());
  
  ll.erase(ll.begin());
  
  DTEST_TRUE(ll.reader_begin() == ll.reader_end());
  
  DTEST_TRUE(ll.reader_begin() == begin);
  
}


  void dtest_queued_deletion() {
  LinkedList<int> ll;
  ll.insert(ll.end(), 1);
  ll.insert(ll.end(), 2);
  ll.insert(ll.end(), 3);
  ll.erase(ll.begin());
  ll.erase(ll.begin());
  ll.erase(ll.begin());
  
  DTEST_TRUE(ll.delete_erased_nodes() == 0);
  
  ll.reader_holds_no_iterator();
  
  DTEST_TRUE(ll.delete_erased_nodes() == 3);

  DTEST_TRUE(ll.delete_erased_nodes() == 0);
}


  void dtest_Iterator() {
  LinkedList<int> ll;
  LinkedList<int> const& ll_const = ll;
  ll.insert(ll.end(), 1);
  ll.insert(ll.end(), 2);
  ll.insert(ll.end(), 3);
  
  LinkedList<int>::Iterator iter = ll.begin();
  
  DTEST_TRUE(iter == ll_const.begin());
  
  DTEST_TRUE(*(iter.operator->()) == 1);
  
  DTEST_TRUE(*iter == 1);
  
  ++iter;
  
  DTEST_TRUE(*iter == 2);
  
  DTEST_TRUE(iter != ll.begin());
  
  --iter;
  
  DTEST_TRUE(*iter == 1);
  
  LinkedList<int>::Iterator iter2 = iter++;
  
  DTEST_TRUE(*iter == 2);
  
  DTEST_TRUE(*iter2 == 1);
  
  iter2 = iter--;
  
  DTEST_TRUE(*iter == 1);
  
  DTEST_TRUE(*iter2 == 2);
  
  LinkedList<int>::ConstIterator c_iter;
  
  DTEST_NOTHROW(c_iter = iter);
  
  DTEST_TRUE((*iter2 = 5) == 5);
  
  typedef std::reverse_iterator<LinkedList<int>::Iterator> RevIter;
  std::ostringstream oss;
  std::copy(RevIter(ll.end()), RevIter(ll.begin()), 
	    std::ostream_iterator<int>(oss));
  
  DTEST_TRUE(oss.str() == "351");
  
  ll.erase(++ll.begin());
  oss.str("");
  std::copy(RevIter(ll.end()), RevIter(ll.begin()), 
	    std::ostream_iterator<int>(oss));
  
  DTEST_TRUE(oss.str() == "31");
}


  void dtest_ConstIterator() {
  LinkedList<int> ll;
  LinkedList<int> const& ll_const = ll;
  ll.insert(ll.end(), 1);
  ll.insert(ll.end(), 2);
  ll.insert(ll.end(), 3);
  
  LinkedList<int>::ConstIterator iter = ll_const.begin();
  
  DTEST_TRUE(iter == ll_const.begin());
  
  DTEST_TRUE(*(iter.operator->()) == 1);
  
  DTEST_TRUE(*iter == 1);
  
  ++iter;
  
  DTEST_TRUE(*iter == 2);
  
  DTEST_TRUE(iter != ll_const.begin());
  
  --iter;
  
  DTEST_TRUE(*iter == 1);
  
  LinkedList<int>::ConstIterator iter2 = iter++;
  
  DTEST_TRUE(*iter == 2);
  
  DTEST_TRUE(*iter2 == 1);
  
  iter2 = iter--;
  
  DTEST_TRUE(*iter == 1);
  
  DTEST_TRUE(*iter2 == 2);
  
  typedef std::reverse_iterator<LinkedList<int>::ConstIterator> RevIter;
  std::ostringstream oss;
  std::copy(RevIter(ll_const.end()), RevIter(ll_const.begin()), 
	    std::ostream_iterator<int>(oss));
  
  DTEST_TRUE(oss.str() == "321");
  
}


  void dtest_ReaderIterator() {
  LinkedList<int> ll;
  ll.insert(ll.end(), 1);
  ll.insert(ll.end(), 2);
  ll.insert(ll.end(), 3);
  
  LinkedList<int>::ReaderIterator iter = ll.reader_begin();
  
  DTEST_TRUE(iter == ll.reader_begin());
  
  DTEST_TRUE(*(iter.operator->()) == 1);
  
  DTEST_TRUE(*iter == 1);
  
  ++iter;
  
  DTEST_TRUE(*iter == 2);
  
  DTEST_TRUE(iter != ll.reader_begin());
  
  LinkedList<int>::ReaderIterator iter2 = iter++;
  
  DTEST_TRUE(*iter == 3);
  
  DTEST_TRUE(*iter2 == 2);
  
  std::ostringstream oss;
  std::copy(ll.reader_begin(), ll.reader_end(), 
	    std::ostream_iterator<int>(oss));
  
  DTEST_TRUE(oss.str() == "123");
  
}


}
