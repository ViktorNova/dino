#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include "debug.hpp"


namespace Dino {


  /** This template class is an implementation of a ringbuffer (or rather
      a circular queue). It can be used to send objects between threads
      in a realtime safe way, assuming that read and write operations on 
      int variables are atomic. */
  template <class T> class Ringbuffer {
  public:
  
    /** This creates a new Ringbuffer object with room for @c size objects. */
    Ringbuffer(int size);
    ~Ringbuffer();
  
    /** This pushes another object onto the queue. */
    bool push(const T& thing);
    /** This removes the last object from the queue and copies it to 
	@c thing. */
    bool pop(T& thing);
  
  protected:
  
    volatile int m_read_pos;
    volatile int m_write_pos;
    const int m_size;
  
    T* volatile m_data;

  };


  template <class T> Ringbuffer<T>::Ringbuffer(int size) 
    : m_read_pos(0),
      m_write_pos(0),
      m_size(size),
      m_data(new T[m_size]) {
    dbg1<<"Creating ringbuffer for "<<demangle(typeid(T).name())<<endl;
  }


  template <class T> Ringbuffer<T>::~Ringbuffer<T>() {
    dbg1<<"Destroying ringbuffer for "<<demangle(typeid(T).name())<<endl;
    delete [] m_data;
  }


  template <class T> bool Ringbuffer<T>::push(const T& thing) {
    if ((m_write_pos + 1) % m_size == m_read_pos)
      return false;
    m_data[m_write_pos] = thing;
    m_write_pos = (m_write_pos + 1) % m_size;
    return true;
  }


  template <class T> bool Ringbuffer<T>::pop(T& thing) {
    if (m_write_pos == m_read_pos)
      return false;
    thing = m_data[m_read_pos];
    m_read_pos = (m_read_pos + 1) % m_size;
    return true;
  }


}


#endif
