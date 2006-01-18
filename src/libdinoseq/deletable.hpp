#ifndef DELETABLE_HPP
#define DELETABLE_HPP

#include "deleter.hpp"


namespace Dino {
  
  /** This base class just exists so we can push pointers to all objects we
      want the sequencer thread to release before deleting on the same queue. */
  class Deletable {
  public:
    /** We need this since the Deleter will work on pointers to this class. */
    virtual ~Deletable() { }
    
    inline bool queue_deletion() {
      return m_deletable_deleter.queue_deletion(this);
    }
    
    inline static Deleter<Deletable>& get_deleter() {
      return m_deletable_deleter;
    }
    
  protected:
    /** This should just be a base class, so no instantiating allowed. */
    Deletable() { }
    
    static Deleter<Deletable> m_deletable_deleter;
    
  };

}


#endif
