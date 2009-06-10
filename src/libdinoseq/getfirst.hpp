#ifndef GETFIRST_HPP
#define GETFIRST_HPP



namespace Dino {
  
  
  /** This is a functor type template that returns the @c first member of a
      pair when called. */
  template <typename K, typename V>
  struct GetFirst {
    typedef K result_type;
    K& operator()(std::pair<K, V>& p) const { return p.first; }
    const K& operator()(const std::pair<K, V>& p) const { return p.first; }
  };


}


#endif
