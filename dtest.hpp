#ifndef DTEST_HPP
#define DTEST_HPP

#include <iomanip>
#include <iostream>
#include <string>


namespace DTest {
  
  extern struct State {
    State() : indent(0) { }
    size_t indent;
    size_t good;
    size_t bad;
  } state;
  
}

/** When we dlopen() ourself, the main program and the dlopened program seem
    to get different copies of the global variables. We get around this by
    setting this pointer (in the dlopened program) to point to DTest::state
    (in the main program). */
extern DTest::State* _dtest;


#define DTEST_LAMBDA(block) [&]() -> void block ()


#define DTEST_INTRO(code, msg)						\
  DTEST_LAMBDA({							\
    std::cout<<std::string(_dtest->indent + 2, ' ')<<"+ "		\
	     <<__FILE__<<':'<<std::setfill('0')<<std::setw(3)<<__LINE__<<": "; \
    std::cout<<'\''<<#code<<"' " msg "... ";				\
  })									\


#define DTEST_SUCCEEDED				\
  DTEST_LAMBDA({				\
    std::cout<<"succeeded"<<endl; ++_dtest->good;	\
  })


#define DTEST_FAILED				\
  DTEST_LAMBDA({				\
    std::cout<<"failed"<<endl; ++_dtest->bad;	\
  })


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


#define DTEST_TRUE(code)					\
  DTEST_LAMBDA({						\
    DTEST_INTRO(code, "should be true");			\
    if (code)							\
      DTEST_SUCCEEDED;						\
    else							\
      DTEST_FAILED;						\
  })


#endif
