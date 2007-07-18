#ifndef ARGUMENT_HPP
#define ARGUMENT_HPP


namespace DBus {

  
  /** This class wraps an argument to a D-Bus method handler.
      @seeAlso DBus::Object::add_method */
  class Argument {
  public:
    
    /** This default constructor creates an Argument object with the type 
	INVALID. */
    Argument();
    /** Creates an integer argument. */
    Argument(int value);
    /** Creates a double precision floating point argument. */
    Argument(double value);
    /** Creates a string argument. */
    Argument(const char* value);
    
    /** The different types that an argument can have. */
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
