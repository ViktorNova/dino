#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP


template <class T> class Ringbuffer {
public:
  Ringbuffer(int size);
  ~Ringbuffer();
  
  bool push(const T& atom);
  bool pop(T& atom);
  
protected:
  
  int m_read_pos;
  int m_write_pos;
  int m_size;
  
  T* m_data;

};


template <class T> Ringbuffer<T>::Ringbuffer<T>(int size) {
  m_data = new T[size];
  m_read_pos = 0;
  m_write_pos = 0;
  m_size = size;
}


template <class T> Ringbuffer<T>::~Ringbuffer<T>() {
  delete m_data;
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


#endif
