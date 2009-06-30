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

#include <limits>
#include <sstream>

#include <boost/test/unit_test.hpp>

#include "songtime.hpp"


using namespace Dino;
using namespace std;


BOOST_AUTO_TEST_SUITE(SongTimeTest)


BOOST_AUTO_TEST_CASE(constructor) {
  SongTime::Beat b = 0;
  SongTime::Tick t = 0;
  BOOST_CHECK_NO_THROW(SongTime st1(b, t));
  BOOST_CHECK_NO_THROW(SongTime st2);
}


BOOST_AUTO_TEST_CASE(get_set) {
  SongTime st;
  
  BOOST_CHECK(st.get_beat() == 0);

  BOOST_CHECK(st.get_tick() == 0);
  
  st.set_beat(-334);
  
  BOOST_CHECK(st.get_beat() == -334);
  
  st.set_tick(6440);
  
  BOOST_CHECK(st.get_tick() == 6440);
  
  st.set_beat(60);
  
  BOOST_CHECK(st.get_beat() == 60);
  
  BOOST_CHECK(st.get_tick() == 6440);
}


BOOST_AUTO_TEST_CASE(equality_inequality) {
  SongTime st1(0, 4);
  SongTime st2(0, 4);
  SongTime st3(4, 344);
  
  BOOST_CHECK(st1 == st2);
  BOOST_CHECK(st1 != st3);
}


BOOST_AUTO_TEST_CASE(compare) {
  SongTime st1(-2, 45);
  SongTime st2(5, 6399);
  SongTime st3(5, 6399);
  
  BOOST_CHECK(st1 < st2);

  BOOST_CHECK(!(st2 < st1));

  BOOST_CHECK(st2 > st1);

  BOOST_CHECK(!(st1 > st2));

  BOOST_CHECK(st1 <= st2);

  BOOST_CHECK(!(st2 <= st1));

  BOOST_CHECK(st2 >= st1);

  BOOST_CHECK(!(st1 >= st2));
  
  BOOST_CHECK(st2 >= st3);

  BOOST_CHECK(st2 <= st3);
}


BOOST_AUTO_TEST_CASE(addition_subtraction) {
  SongTime st1(0, 0);
  
  BOOST_CHECK(st1 + st1 == st1);
  
  BOOST_CHECK(st1 + SongTime(34, 2) == SongTime(34, 2));
  
  BOOST_CHECK(st1 - st1 == st1);
  
  BOOST_CHECK(st1 - SongTime(34, 2) == 
	      SongTime(-35, numeric_limits<SongTime::Tick>::max() - 1));
  
  st1 += SongTime(45, 3);
  
  BOOST_CHECK(st1 == SongTime(45, 3));
  
  st1 += SongTime(-3, 2);
  
  BOOST_CHECK(st1 == SongTime(42, 5));
  
  st1 -= SongTime(-3, 0);
  
  BOOST_CHECK(st1 == SongTime(45, 5));
  
  st1 -= SongTime(5, 1);
  
  BOOST_CHECK(st1 == SongTime(40, 4));
}


BOOST_AUTO_TEST_CASE(ostream) {
  SongTime st(0x29A, 0x449783);
  ostringstream os;
  
  os<<st<<flush;
  
  BOOST_CHECK(os.str() == "29A:00449783");
}


BOOST_AUTO_TEST_SUITE_END()
