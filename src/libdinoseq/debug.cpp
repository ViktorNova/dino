#include "debug.hpp"


namespace Dino {


#ifdef NDEBUG 
  
  ostream& dbg0(cerr);
  
  NoOpStream dbg1;
  
#endif


}
