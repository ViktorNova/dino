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
#include <vector>

#include <boost/test/unit_test.hpp>

#include "curve.hpp"


using namespace Dino;


/* We can't really test the atomicity in any reasonable way, so we do some
   read and write tests. */

BOOST_AUTO_TEST_SUITE(CurveTest)


BOOST_AUTO_TEST_CASE(constructor) {
  BOOST_CHECK_NO_THROW(Curve c("Test curve", SongTime(1, 0), 1));
}


BOOST_AUTO_TEST_CASE(get_set_controller_id) {
  Curve c("Test curve", SongTime(4, 0), 1);
  
  BOOST_CHECK(c.get_controller_id() == 1);
  
  c.set_controller_id(42);
  
  BOOST_CHECK(c.get_controller_id() == 42);
}


BOOST_AUTO_TEST_CASE(add_move_remove_point) {
  Curve c("Test curve", SongTime(4, 0), 1);
  
  BOOST_CHECK_THROW(c.add_point(SongTime(5, 0), 666), std::out_of_range);
  
  Curve::Iterator iter1 = c.add_point(SongTime(0, 0), 666);
  
  BOOST_CHECK(iter1 != c.end());
  
  Curve::Iterator iter2 = c.add_point(SongTime(1, 0), 420);
  
  BOOST_CHECK(iter2 != c.end());
  
  BOOST_CHECK(iter1 != iter2);
  
  BOOST_CHECK(iter1 == c.begin());
  
  Curve::Iterator iter3 = c.add_point(SongTime(0, 0), 28, iter1);
  
  BOOST_CHECK(iter3 != c.end());
  
  BOOST_CHECK(iter3 != iter2);

  BOOST_CHECK(iter3 != iter1);
  
  BOOST_CHECK_THROW(c.add_point(SongTime(3, 0), 0, iter2), 
		    std::invalid_argument);
  
  BOOST_CHECK_THROW(c.move_point(iter1, SongTime(2, 0), 666),
		    std::out_of_range);
  
  BOOST_CHECK_THROW(c.move_point(iter1, SongTime(-1, 0), 666),
		    std::out_of_range);

  iter1 = c.move_point(iter1, SongTime(0, 5), 666);
  
  BOOST_CHECK(iter1 != c.end());
  
  BOOST_CHECK(c.remove_point(iter1) == iter2);
  
  BOOST_CHECK(c.remove_point(iter3) == iter2);
  
  BOOST_CHECK(c.remove_point(iter2) == c.end());
}


BOOST_AUTO_TEST_CASE(begin_end) {
  Curve c("Test curve", SongTime(4, 0), 1);
  
  Curve::Iterator begin = c.begin();
  
  BOOST_CHECK(c.begin() == c.end());

  Curve::Iterator iter = c.add_point(SongTime(0, 0), 0);
  
  BOOST_CHECK(begin != c.begin());
  
  BOOST_CHECK(c.begin() != c.end());
  
  c.remove_point(iter);
  
  BOOST_CHECK(c.begin() == c.end());
  
  BOOST_CHECK(c.begin() == begin);
}


BOOST_AUTO_TEST_CASE(begin_end_const) {
  Curve c("Test curve", SongTime(4, 0), 1);
  Curve const& cc = c;
  
  Curve::Iterator begin = c.begin();
  
  BOOST_CHECK(cc.begin() == cc.end());

  Curve::Iterator iter = c.add_point(SongTime(0, 0), 0);
  
  BOOST_CHECK(begin != cc.begin());
  
  BOOST_CHECK(cc.begin() != cc.end());
  
  c.remove_point(iter);
  
  BOOST_CHECK(cc.begin() == cc.end());
  
  BOOST_CHECK(cc.begin() == begin);
}


BOOST_AUTO_TEST_CASE(lower_upper_bound) {
  Curve c("Test curve", SongTime(4, 0), 1);
  
  c.add_point(SongTime(0, 0), 0);
  Curve::Iterator iter1 = c.add_point(SongTime(1, 0), 0);
  c.add_point(SongTime(1, 0), 0);
  Curve::Iterator iter2 = c.add_point(SongTime(2, 0), 0);
  c.add_point(SongTime(3, 0), 0);
  
  BOOST_CHECK(c.lower_bound(SongTime(1, 0)) == iter1);
  
  BOOST_CHECK(c.upper_bound(SongTime(1, 0)) == iter2);
  
  BOOST_CHECK(c.lower_bound(SongTime(4, 0)) == c.end());
}


BOOST_AUTO_TEST_CASE(lower_upper_bound_const) {
  Curve c("Test curve", SongTime(4, 0), 1);
  Curve const& cc = c;
  
  c.add_point(SongTime(0, 0), 0);
  Curve::Iterator iter1 = c.add_point(SongTime(1, 0), 0);
  c.add_point(SongTime(1, 0), 0);
  Curve::Iterator iter2 = c.add_point(SongTime(2, 0), 0);
  c.add_point(SongTime(3, 0), 0);
  
  BOOST_CHECK(cc.lower_bound(SongTime(1, 0)) == iter1);
  
  BOOST_CHECK(cc.upper_bound(SongTime(1, 0)) == iter2);
  
  BOOST_CHECK(cc.lower_bound(SongTime(4, 0)) == c.end());
}


BOOST_AUTO_TEST_CASE(Iterator) {
  Curve c("Test curve", SongTime(4, 0), 1);
  Curve const& cc = c;
  
  c.add_point(SongTime(1, 0), 0);
  c.add_point(SongTime(2, 0), 0);
  c.add_point(SongTime(3, 0), 0);
  
  Curve::Iterator iter = c.begin();
  
  BOOST_CHECK(iter == cc.begin());
  
  BOOST_CHECK(iter.operator->()->m_time == SongTime(1, 0));
  
  BOOST_CHECK(iter->m_time == SongTime(1, 0));
  
  ++iter;
  
  BOOST_CHECK(iter->m_time == SongTime(2, 0));
  
  BOOST_CHECK(iter != c.begin());
  
  --iter;
  
  BOOST_CHECK(iter->m_time == SongTime(1, 0));
  
  Curve::Iterator iter2 = iter++;
  
  BOOST_CHECK(iter->m_time == SongTime(2, 0));
  
  BOOST_CHECK(iter2->m_time == SongTime(1, 0));
  
  iter2 = iter--;
  
  BOOST_CHECK(iter->m_time == SongTime(1, 0));
  
  BOOST_CHECK(iter2->m_time == SongTime(2, 0));
  
  Curve::ConstIterator c_iter;
  
  BOOST_CHECK_NO_THROW(c_iter = iter);
  
  typedef std::reverse_iterator<Curve::Iterator> RevIter;
  std::vector<Curve::Point> ps;
  std::copy(RevIter(c.end()), RevIter(c.begin()), 
	    std::back_inserter(ps));
  
  BOOST_CHECK(ps.size() == 3);
  
  BOOST_CHECK(ps[0].m_time == SongTime(3, 0));

  BOOST_CHECK(ps[1].m_time == SongTime(2, 0));

  BOOST_CHECK(ps[2].m_time == SongTime(1, 0));
  
  c.remove_point(++c.begin());
  
  ps.clear();
  std::copy(RevIter(c.end()), RevIter(c.begin()), 
	    std::back_inserter(ps));
  
  BOOST_CHECK(ps.size() == 2);
  
  BOOST_CHECK(ps[0].m_time == SongTime(3, 0));

  BOOST_CHECK(ps[1].m_time == SongTime(1, 0));
}


BOOST_AUTO_TEST_CASE(ConstIterator) {
  Curve c("Test curve", SongTime(4, 0), 1);
  Curve const& cc = c;
  
  c.add_point(SongTime(1, 0), 0);
  c.add_point(SongTime(2, 0), 0);
  c.add_point(SongTime(3, 0), 0);
  
  Curve::ConstIterator iter = cc.begin();
  
  BOOST_CHECK(iter == c.begin());
  
  BOOST_CHECK(iter.operator->()->m_time == SongTime(1, 0));
  
  BOOST_CHECK(iter->m_time == SongTime(1, 0));
  
  ++iter;
  
  BOOST_CHECK(iter->m_time == SongTime(2, 0));
  
  BOOST_CHECK(iter != cc.begin());
  
  --iter;
  
  BOOST_CHECK(iter->m_time == SongTime(1, 0));
  
  Curve::ConstIterator iter2 = iter++;
  
  BOOST_CHECK(iter->m_time == SongTime(2, 0));
  
  BOOST_CHECK(iter2->m_time == SongTime(1, 0));
  
  iter2 = iter--;
  
  BOOST_CHECK(iter->m_time == SongTime(1, 0));
  
  BOOST_CHECK(iter2->m_time == SongTime(2, 0));
  
  Curve::ConstIterator c_iter;
  
  BOOST_CHECK_NO_THROW(c_iter = iter);
}


BOOST_AUTO_TEST_SUITE_END()
