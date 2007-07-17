/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

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
  
    /** This pushes another object onto the queue. Returns @c false if the
        queue is full. */
    bool push(const T& thing);
    /** This removes the last object from the queue and copies it to 
        @c thing. Returns @c false if the queue is empty. */
    bool pop(T& thing);
    /** This removes the last object from the queue and returns a copy of it. 
        If the queue is empty the behaviour is undefined. */
    T pop();
    
    /** This clears the buffer (not threadsafe!) */
    void clear();
  
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
    Dino::dbg1<<"Creating ringbuffer for "
	      <<Dino::demangle(typeid(T).name())<<std::endl;
  }


  template <class T> Ringbuffer<T>::~Ringbuffer<T>() {
    Dino::dbg1<<"Destroying ringbuffer for "
	      <<Dino::demangle(typeid(T).name())<<std::endl;
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


  template <class T> T Ringbuffer<T>::pop() {
    T thing;
    pop(thing);
    return thing;
  }


  template <class T> void Ringbuffer<T>::clear() {
    m_write_pos = 0;
    m_read_pos = 0;
  }


}


#endif
