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

#include <limits>
#include <sstream>

#include "dtest.hpp"
#include "songtime.hpp"


using namespace Dino;
using namespace std;


namespace SongTimeTest {


  void dtest_constructor() {
    SongTime::Beat b = 0;
    SongTime::Tick t = 0;
    DTEST_NOTHROW(SongTime st1(b, t));
    DTEST_NOTHROW(SongTime st2);
  }


  void dtest_get_set() {
    SongTime st;
  
    DTEST_TRUE(st.get_beat() == 0);

    DTEST_TRUE(st.get_tick() == 0);
  
    st.set_beat(-334);
  
    DTEST_TRUE(st.get_beat() == -334);
  
    st.set_tick(6440);
  
    DTEST_TRUE(st.get_tick() == 6440);
  
    st.set_beat(60);
  
    DTEST_TRUE(st.get_beat() == 60);
  
    DTEST_TRUE(st.get_tick() == 6440);
  }


  void dtest_equality_inequality() {
    SongTime st1(0, 4);
    SongTime st2(0, 4);
    SongTime st3(4, 344);
  
    DTEST_TRUE(st1 == st2);
    DTEST_TRUE(st1 != st3);
  }


  void dtest_compare() {
    SongTime st1(-2, 45);
    SongTime st2(5, 6399);
    SongTime st3(5, 6399);
  
    DTEST_TRUE(st1 < st2);

    DTEST_TRUE(!(st2 < st1));

    DTEST_TRUE(st2 > st1);

    DTEST_TRUE(!(st1 > st2));

    DTEST_TRUE(st1 <= st2);

    DTEST_TRUE(!(st2 <= st1));

    DTEST_TRUE(st2 >= st1);

    DTEST_TRUE(!(st1 >= st2));
  
    DTEST_TRUE(st2 >= st3);

    DTEST_TRUE(st2 <= st3);
  }


  void dtest_addition_subtraction() {
    SongTime st1(0, 0);
  
    DTEST_TRUE(st1 + st1 == st1);
  
    DTEST_TRUE(st1 + SongTime(34, 2) == SongTime(34, 2));
  
    DTEST_TRUE(st1 - st1 == st1);
  
    DTEST_TRUE(st1 - SongTime(34, 2) == 
	       SongTime(-35, SongTime::ticks_per_beat() - 1));
  
    st1 += SongTime(45, 3);
  
    DTEST_TRUE(st1 == SongTime(45, 3));
  
    st1 += SongTime(-3, 2);
  
    DTEST_TRUE(st1 == SongTime(42, 5));
  
    st1 -= SongTime(-3, 0);
  
    DTEST_TRUE(st1 == SongTime(45, 5));
  
    st1 -= SongTime(5, 1);
  
    DTEST_TRUE(st1 == SongTime(40, 4));
  }


  void dtest_ostream() {
    SongTime st(0x29A, 0x449783);
    ostringstream os;
  
    os<<st<<flush;
  
    DTEST_TRUE(os.str() == "29A:449783");
  }


}
