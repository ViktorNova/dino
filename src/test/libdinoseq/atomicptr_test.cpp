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

#include "atomicptr.hpp"


using namespace Dino;


/* We can't really test the atomicity in any reasonable way, so we just
   do some trivial read and write tests. */

BOOST_AUTO_TEST_SUITE(AtomicPtrTest)


BOOST_AUTO_TEST_CASE(constructor) {
  int* iptr = new int(42);
  BOOST_CHECK_NO_THROW(AtomicPtr<int> ap(iptr));
  delete iptr;
}


BOOST_AUTO_TEST_CASE(get_set) {
  int a = 42;
  int b = 666;
  AtomicPtr<int> ap = &a;
  
  BOOST_CHECK_EQUAL(&a, ap.get());
  
  ap.set(&b);
  
  BOOST_CHECK_EQUAL(&b, ap.get());
  
  AtomicPtr<int> const ap_c = &b;

  BOOST_CHECK_EQUAL(&b, ap_c.get());
}


BOOST_AUTO_TEST_SUITE_END()
