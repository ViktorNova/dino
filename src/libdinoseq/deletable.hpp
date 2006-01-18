#ifndef DELETABLE_HPP
#define DELETABLE_HPP


namespace Dino {
  
  /** This base class just exists so we can push pointers to all objects we
      want the sequencer thread to release before deleting on the same queue. */
  class Deletable {
  public:
    /** We need this since the Deleter will work on pointers to this class. */
    virtual ~Deletable() { }
    
  protected:
    /** This should just be a base class, so no instantiating allowed. */
    Deletable() { }
  };

}


#endif
