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

#include "atomicint.hpp"
#include "dtest.hpp"


using namespace Dino;


/* We can't really test the atomicity in any reasonable way, so we just
   do some trivial read and write tests. */

namespace AtomicIntTest {


  void dtest_constructor(){
    AtomicInt::Type i = 3;
    DTEST_NOTHROW(AtomicInt ai(i));
  }


  void dtest_get_set() {
    AtomicInt::Type a = 42;
    AtomicInt::Type b = 666;
    AtomicInt ai = a;
  
    DTEST_TRUE(a == ai.get());
  
    ai.set(b);
    
    DTEST_TRUE(b == ai.get());
  }


  void dtest_increase() {
    AtomicInt::Type a = 42;
    AtomicInt ai = a;
    ai.increase();
    
    DTEST_TRUE(++a == ai.get());
  }


}
