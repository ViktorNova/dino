#include "argument.hpp"


namespace DBus {


  Argument::Argument()
    : type(INVALID) {

  }
  
  
  Argument::Argument(int value)
    : type(INT),
      i(value) {

  }
  
  
  Argument::Argument(double value)
    : type(DOUBLE),
      d(value) {

  }
  
  
  Argument::Argument(const char* value)
    : type(STRING),
      s(value) {

  }


}
