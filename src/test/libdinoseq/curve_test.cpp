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
#include <vector>

#include "dtest.hpp"
#include "curve.hpp"


using namespace Dino;


/* We can't really test the atomicity in any reasonable way, so we do some
   read and write tests. */

namespace CurveTest {


  void dtest_constructor() {
    DTEST_NOTHROW(Curve c("Test curve", SongTime(1, 0), 1));
  }


  void dtest_get_set_controller_id() {
    Curve c("Test curve", SongTime(4, 0), 1);
    
    DTEST_TRUE(c.get_controller_id() == 1);
    
    c.set_controller_id(42);
    
    DTEST_TRUE(c.get_controller_id() == 42);
  }


  void dtest_add_move_remove_point() {
    Curve c("Test curve", SongTime(4, 0), 1);
    
    DTEST_THROW_TYPE(c.add_point(SongTime(5, 0), 666), std::out_of_range);
  
  Curve::Iterator iter1 = c.add_point(SongTime(0, 0), 666);
  
  DTEST_TRUE(iter1 != c.end());
  
  Curve::Iterator iter2 = c.add_point(SongTime(1, 0), 420);
  
  DTEST_TRUE(iter2 != c.end());
  
  DTEST_TRUE(iter1 != iter2);
  
  DTEST_TRUE(iter1 == c.begin());
  
  Curve::Iterator iter3 = c.add_point(SongTime(0, 0), 28, iter1);
  
  DTEST_TRUE(iter3 != c.end());
  
  DTEST_TRUE(iter3 != iter2);

  DTEST_TRUE(iter3 != iter1);
  
  DTEST_THROW_TYPE(c.add_point(SongTime(3, 0), 0, iter2), 
		    std::invalid_argument);
  
  DTEST_THROW_TYPE(c.move_point(iter1, SongTime(2, 0), 666),
		    std::out_of_range);
  
  DTEST_THROW_TYPE(c.move_point(iter1, SongTime(-1, 0), 666),
		    std::out_of_range);

  iter1 = c.move_point(iter1, SongTime(0, 5), 666);
  
  DTEST_TRUE(iter1 != c.end());
  
  DTEST_TRUE(c.remove_point(iter1) == iter2);
  
  DTEST_TRUE(c.remove_point(iter3) == iter2);
  
  DTEST_TRUE(c.remove_point(iter2) == c.end());
}


  void dtest_begin_end() {
  Curve c("Test curve", SongTime(4, 0), 1);
  
  Curve::Iterator begin = c.begin();
  
  DTEST_TRUE(c.begin() == c.end());

  Curve::Iterator iter = c.add_point(SongTime(0, 0), 0);
  
  DTEST_TRUE(begin != c.begin());
  
  DTEST_TRUE(c.begin() != c.end());
  
  c.remove_point(iter);
  
  DTEST_TRUE(c.begin() == c.end());
  
  DTEST_TRUE(c.begin() == begin);
}


  void dtest_begin_end_const() {
  Curve c("Test curve", SongTime(4, 0), 1);
  Curve const& cc = c;
  
  Curve::Iterator begin = c.begin();
  
  DTEST_TRUE(cc.begin() == cc.end());

  Curve::Iterator iter = c.add_point(SongTime(0, 0), 0);
  
  DTEST_TRUE(begin != cc.begin());
  
  DTEST_TRUE(cc.begin() != cc.end());
  
  c.remove_point(iter);
  
  DTEST_TRUE(cc.begin() == cc.end());
  
  DTEST_TRUE(cc.begin() == begin);
}


  void dtest_lower_upper_bound() {
    Curve c("Test curve", SongTime(4, 0), 1);
    
    c.add_point(SongTime(0, 0), 0);
    Curve::Iterator iter1 = c.add_point(SongTime(1, 0), 0);
    c.add_point(SongTime(1, 0), 0);
    Curve::Iterator iter2 = c.add_point(SongTime(2, 0), 0);
    c.add_point(SongTime(3, 0), 0);
    
    DTEST_TRUE(c.lower_bound(SongTime(1, 0)) == iter1);
    
    DTEST_TRUE(c.upper_bound(SongTime(1, 0)) == iter2);
    
    DTEST_TRUE(c.lower_bound(SongTime(4, 0)) == c.end());
  }


  void dtest_lower_upper_bound_const() {
    Curve c("Test curve", SongTime(4, 0), 1);
    Curve const& cc = c;
    
    c.add_point(SongTime(0, 0), 0);
    Curve::Iterator iter1 = c.add_point(SongTime(1, 0), 0);
    c.add_point(SongTime(1, 0), 0);
    Curve::Iterator iter2 = c.add_point(SongTime(2, 0), 0);
    c.add_point(SongTime(3, 0), 0);
    
    DTEST_TRUE(cc.lower_bound(SongTime(1, 0)) == iter1);
    
    DTEST_TRUE(cc.upper_bound(SongTime(1, 0)) == iter2);
    
    DTEST_TRUE(cc.lower_bound(SongTime(4, 0)) == c.end());
  }
  
  
  void dtest_Iterator() {
    Curve c("Test curve", SongTime(4, 0), 1);
    Curve const& cc = c;
    
    c.add_point(SongTime(1, 0), 0);
    c.add_point(SongTime(2, 0), 0);
    c.add_point(SongTime(3, 0), 0);
    
    Curve::Iterator iter = c.begin();
    
    DTEST_TRUE(iter == cc.begin());
    
    DTEST_TRUE(iter.operator->()->m_time == SongTime(1, 0));
    
    DTEST_TRUE(iter->m_time == SongTime(1, 0));
    
    ++iter;
    
    DTEST_TRUE(iter->m_time == SongTime(2, 0));
    
    DTEST_TRUE(iter != c.begin());
    
    --iter;
    
    DTEST_TRUE(iter->m_time == SongTime(1, 0));
    
    Curve::Iterator iter2 = iter++;
    
    DTEST_TRUE(iter->m_time == SongTime(2, 0));
    
    DTEST_TRUE(iter2->m_time == SongTime(1, 0));
    
    iter2 = iter--;
    
    DTEST_TRUE(iter->m_time == SongTime(1, 0));
    
    DTEST_TRUE(iter2->m_time == SongTime(2, 0));
    
    Curve::ConstIterator c_iter;
    
    DTEST_NOTHROW(c_iter = iter);
    
    typedef std::reverse_iterator<Curve::Iterator> RevIter;
    std::vector<Curve::Point> ps;
    std::copy(RevIter(c.end()), RevIter(c.begin()), 
	      std::back_inserter(ps));
    
    DTEST_TRUE(ps.size() == 3);
    
    DTEST_TRUE(ps[0].m_time == SongTime(3, 0));
    
    DTEST_TRUE(ps[1].m_time == SongTime(2, 0));
    
    DTEST_TRUE(ps[2].m_time == SongTime(1, 0));
    
    c.remove_point(++c.begin());
    
    ps.clear();
    std::copy(RevIter(c.end()), RevIter(c.begin()), 
	      std::back_inserter(ps));
    
    DTEST_TRUE(ps.size() == 2);
    
    DTEST_TRUE(ps[0].m_time == SongTime(3, 0));
    
    DTEST_TRUE(ps[1].m_time == SongTime(1, 0));
  }
  
  
  void dtest_ConstIterator() {
    Curve c("Test curve", SongTime(4, 0), 1);
    Curve const& cc = c;
    
    c.add_point(SongTime(1, 0), 0);
    c.add_point(SongTime(2, 0), 0);
    c.add_point(SongTime(3, 0), 0);
    
    Curve::ConstIterator iter = cc.begin();
    
    DTEST_TRUE(iter == c.begin());
    
    DTEST_TRUE(iter.operator->()->m_time == SongTime(1, 0));
    
    DTEST_TRUE(iter->m_time == SongTime(1, 0));
    
    ++iter;
    
    DTEST_TRUE(iter->m_time == SongTime(2, 0));
    
    DTEST_TRUE(iter != cc.begin());
    
    --iter;
    
    DTEST_TRUE(iter->m_time == SongTime(1, 0));
    
    Curve::ConstIterator iter2 = iter++;
    
    DTEST_TRUE(iter->m_time == SongTime(2, 0));
    
    DTEST_TRUE(iter2->m_time == SongTime(1, 0));
    
    iter2 = iter--;
    
    DTEST_TRUE(iter->m_time == SongTime(1, 0));
    
    DTEST_TRUE(iter2->m_time == SongTime(2, 0));
    
    Curve::ConstIterator c_iter;
    
    DTEST_NOTHROW(c_iter = iter);
  }


}
