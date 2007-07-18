#ifndef ARGUMENT_HPP
#define ARGUMENT_HPP


namespace DBus {


  class Argument {
  public:
    
    Argument();
    Argument(int value);
    Argument(double value);
    Argument(const char* value);
    
    enum Type {
      INT,
      DOUBLE,
      STRING,
      INVALID
    } type;
    union {
      int i;
      double d;
      const char* s;
    };
  };


}


#endif
