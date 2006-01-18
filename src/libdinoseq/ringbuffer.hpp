#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include "debug.hpp"


namespace Dino {


  /** This template class is an implementation of a ringbuffer (or rather
      a circular queue). It can be used to send objects between threads
      in a realtime safe way. */
  template <class T> class Ringbuffer {
  public:
  
    /** This creates a new Ringbuffer object with room for @c size objects. */
    Ringbuffer(int size);
    ~Ringbuffer();
  
    /** This pushes another object onto the queue. */
    bool push(const T& atom);
    /** This removes the last object from the queue and copies it to 
	@c atom. */
    bool pop(T& atom);
  
  protected:
  
    volatile int m_read_pos;
    volatile int m_write_pos;
    int m_size;
  
    volatile T* m_data;

  };


  template <class T> Ringbuffer<T>::Ringbuffer(int size) {
    dbg1<<"Creating ringbuffer for "<<demangle(typeid(T).name())<<endl;
    m_data = new T[size];
    m_read_pos = 0;
    m_write_pos = 0;
    m_size = size;
  }


  template <class T> Ringbuffer<T>::~Ringbuffer<T>() {
    dbg1<<"Destroying ringbuffer for "<<demangle(typeid(T).name())<<endl;
    delete [] m_data;
  }


  template <class T> bool Ringbuffer<T>::push(const T& atom) {
    if ((m_write_pos + 1) % m_size == m_read_pos)
      return false;
    m_data[m_write_pos] = atom;
    m_write_pos = (m_write_pos + 1) % m_size;
    return true;
  }


  template <class T> bool Ringbuffer<T>::pop(T& atom) {
    if (m_write_pos == m_read_pos)
      return false;
    atom = m_data[m_read_pos];
    m_read_pos = (m_read_pos + 1) % m_size;
    return true;
  }


}


#endif
