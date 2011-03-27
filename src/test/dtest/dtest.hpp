/*****************************************************************************
    DTest - a simple testing framework for C++
    Copyright (C) 2011  Lars Luthman <mail@larsluthman.net>
    
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

#ifndef DTEST_HPP
#define DTEST_HPP

#include <iomanip>
#include <iostream>
#include <string>

/** @mainpage
    This is the documentation for DTest, a simple testing framework for
    unit tests and regression tests. See the documentation for dtest.hpp
    for details.
*/


/** @file
    This is a very simple testing framework for unit and regression tests.
    You create a test program by either compiling and linking @c dtest.cpp with
    your tests, or compiling @c dtest.cpp as a separate program and compiling
    and linking your tests as a shared library, the path of which you then pass
    as the only command line parameter to the @c dtest program. @c dtest.cpp
    defines a @c main() function that will run your tests and print a report.
    
    Your tests should be free functions taking no arguments and returning
    @c void, and their names should begin with @c dtest_. You can arrange them
    into nested test suites using namespaces, like this:

    @code
    namespace MyTests {
    
      void dtest_first_test() {
        DTEST_TRUE(1 == 2);
      }
      
      namespace NestedSuite {
      
        void dtest_another_test() {
          DTEST_NOTHROW(this_function_does_not_throw());
	}

      }
    
    }
    @endcode
    
    Test suites and tests are run in alphabetical order, and nested suites are
    run as a part of their parent suites.
    
    You can test the truth value of a code snippet (using DTEST_TRUE()), 
    that it doesn't throw an exception (DTEST_NOTHROW()), that it <em>does</em>
    throw an exception (DTEST_THROW()), and that it throws an exception of
    a certain type (DTEST_THROW_TYPE()).
    
    Test programs need to be compiled with @c -fPIC and @c -pie, and linked with
    @c -fPIC, @c -pie, @c -ldl and @c -rdynamic. This is because the program
    will @c dlopen() itself to get pointers to the test functions.
    
    If you link your tests into separate shared libraries, they need to be
    compiled with @c -fPIC and linked with @c -shared @c -fPIC as per usual.
    
    The test program must not be stripped. Also, your default shell (the one
    used by @c popen()) must have the programs @c awk, @c c++filt,
    @c grep, @c nm, @c sed, @c sh and @c xargs available, and your dynamic
    linker must support backlinking.
*/


/** @internal
    Namespace for internal things. */
namespace DTest {
  
  /** @internal
      An object of this type is used to store the state of the test. */
  extern struct State {
    
    /** @internal
	Resets the indentation to 0. */
    State() : indent(0), good(0), bad(0) { }
    
    /** @internal
	The current indentation level. */
    size_t indent;

    /** @internal
	The good counter. */
    size_t good;

    /** @internal
	The bad counter. */
    size_t bad;
  } state;
  
}


/** @internal
    When we dlopen() ourself, the main program and the dlopened program seem
    to get different copies of the global variables. We get around this by
    setting this pointer (in the dlopened program) to point to DTest::state
    (in the main program). */
extern DTest::State* _dtest;


/** @internal
    Run the given code block (must be enclosed in curly brackets) as a lambda
    function. */
#define DTEST_LAMBDA(block) [&]() -> void block ()


/** @internal
    Print an introduction message for a test. */
#define DTEST_INTRO(code, msg)						\
  DTEST_LAMBDA({							\
    std::cout<<std::string(_dtest->indent + 2, ' ')<<"+ "		\
	     <<__FILE__<<':'<<std::setfill('0')<<std::setw(3)<<__LINE__<<": "; \
    std::cout<<'\''<<#code<<"' " msg "... ";				\
  })									\


/** Print a test message using the current indentation level. */
#define DTEST_MSG(msg)							\
  DTEST_LAMBDA({							\
    std::cout<<std::string(_dtest->indent + 2, ' ')<<(msg)<<std::endl; \
  })


/** @internal
    Bump the good counter and print "succeeded". */
#define DTEST_SUCCEEDED				\
  DTEST_LAMBDA({				\
    std::cout<<"succeeded"<<endl; ++_dtest->good;	\
  })


/** @internal
    Bump the bad counter and print "failed". */
#define DTEST_FAILED				\
  DTEST_LAMBDA({				\
    std::cout<<"failed"<<endl; ++_dtest->bad;	\
  })


/** This test succeeds if the code snippet @c code does not throw.
    @c code should be a list of statements, separated by @c ;, with no
    trailing @c ;. */
#define DTEST_NOTHROW(code)					\
  DTEST_LAMBDA({						\
    DTEST_INTRO(code, "should not throw");			\
    try {							\
      code;							\
      DTEST_SUCCEEDED;						\
    }								\
    catch (...) {						\
      DTEST_FAILED;						\
    }								\
  })


/** This test succeeds if the code snippet @c code throws something.
    @c code should be a list of statements, separated by @c ;, with no
    trailing @c ;. */
#define DTEST_THROW(code)			\
  DTEST_LAMBDA({				\
    DTEST_INTRO(code, "should throw");		\
    try {					\
      code;					\
      DTEST_FAILED;				\
    }						\
    catch (...) {				\
      DTEST_SUCCEEDED;				\
    }\
  })


/** This test succeeds if the code snippet @c code throws a @c type.
    @c code should be a list of statements, separated by @c ;, with no
    trailing @c ;. */
#define DTEST_THROW_TYPE(code, type)		\
  DTEST_LAMBDA({				\
    DTEST_INTRO(code, "should throw " #type);	\
    try {					\
      code;					\
      DTEST_FAILED;				\
    }						\
    catch (type& e) {				\
      DTEST_SUCCEEDED;				\
    }						\
    catch (...) {				\
      DTEST_FAILED;				\
    }						\
  })


/** This test succeeds if the expression @c code evaluates to @c true.
    @c code should be an expression with no trailing @c ;. */
#define DTEST_TRUE(code)						\
  DTEST_LAMBDA({							\
    DTEST_INTRO(code, "should be true");				\
    try {								\
      if (code)								\
	DTEST_SUCCEEDED;						\
      else								\
	DTEST_FAILED;							\
    }									\
    catch (std::exception& e) {						\
      DTEST_FAILED;							\
      DTEST_MSG(std::string("    Unexpected ") +			\
		typeid(e).name() + " thrown: '" + e.what() + "'");	\
    }									\
    catch (...) {						\
      DTEST_FAILED;							\
      DTEST_MSG(std::string("    Unexpected object of unknown type thrown"));\
    }									\
  })


#endif
