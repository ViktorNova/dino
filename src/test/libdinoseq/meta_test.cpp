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

#include "meta.hpp"


using namespace Dino;


BOOST_AUTO_TEST_SUITE(MetaTest)


BOOST_AUTO_TEST_CASE(is_same_test) {
  BOOST_CHECK((is_same<int, int>::value));
  
  BOOST_CHECK((!is_same<int, float>::value));
  
  BOOST_CHECK((!is_same<int, int const>::value));
  
  BOOST_CHECK((!is_same<int const*, int*>::value));
}


BOOST_AUTO_TEST_CASE(const_if_test) {
  BOOST_CHECK((is_same<const_if<int, false>::type, int>::value));
  
  BOOST_CHECK((is_same<const_if<int, true>::type, int const>::value));
  
  BOOST_CHECK((is_same<const_if<int*, false>::type, int*>::value));

  BOOST_CHECK((is_same<const_if<int*, true>::type, int* const>::value));

  BOOST_CHECK((is_same<const_if<int* const, false>::type, int* const>::value));
}


BOOST_AUTO_TEST_CASE(copy_const_test) {
  BOOST_CHECK((is_same<copy_const<int const, float>::type,
	       float const>::value));

  BOOST_CHECK((is_same<copy_const<int, float>::type
	       , float>::value));

  BOOST_CHECK((is_same<copy_const<int const, float const>::type, 
	       float const>::value));

BOOST_CHECK((is_same<copy_const<int, float const>::type,
	     float const>::value));
}


BOOST_AUTO_TEST_SUITE_END()
