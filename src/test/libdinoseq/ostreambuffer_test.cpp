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

#include <sstream>

#include "dtest.hpp"
#include "ostreambuffer.hpp"
#include "songtime.hpp"


using namespace Dino;
using namespace std;


namespace OStreamBufferTest {


  void dtest_constructor() {
    ostringstream os;
    DTEST_NOTHROW(OStreamBuffer osb(os));
  }


  void dtest_write_event() {
    ostringstream os;
    OStreamBuffer osb(os);
    
    unsigned char event1[] = {0x80, 0x34, 0x00};
    unsigned char event2[] = {0x90, 0x34, 0x42};
    
    SongTime st1(0, 0x238388);
    SongTime st2(5, 0xFFAD03);
    
    osb.write_event(st1, 3, event1);
    osb.write_event(st2, 3, event2);
    
    os<<flush;
    
    DTEST_TRUE(os.str() == "0:238388: 80 34 00\n5:FFAD03: 90 34 42\n");
  }


}
