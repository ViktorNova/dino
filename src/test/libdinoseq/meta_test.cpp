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

#include "dtest.hpp"
#include "meta.hpp"


using namespace Dino;


namespace MetaTest {


  void dtest_is_same_test() {
    DTEST_TRUE((is_same<int, int>::value));
    
    DTEST_TRUE((!is_same<int, float>::value));
    
    DTEST_TRUE((!is_same<int, int const>::value));
    
    DTEST_TRUE((!is_same<int const*, int*>::value));
  }


  void dtest_const_if_test() {
    DTEST_TRUE((is_same<const_if<int, false>::type, int>::value));
    
    DTEST_TRUE((is_same<const_if<int, true>::type, int const>::value));
    
    DTEST_TRUE((is_same<const_if<int*, false>::type, int*>::value));
    
    DTEST_TRUE((is_same<const_if<int*, true>::type, int* const>::value));
    
    DTEST_TRUE((is_same<const_if<int* const, false>::type, int* const>::value));
  }


  void dtest_copy_const_test() {
    DTEST_TRUE((is_same<copy_const<int const, float>::type,
		float const>::value));
    
    DTEST_TRUE((is_same<copy_const<int, float>::type
		, float>::value));
    
    DTEST_TRUE((is_same<copy_const<int const, float const>::type, 
		float const>::value));
    
    DTEST_TRUE((is_same<copy_const<int, float const>::type,
		float const>::value));
  }


}
