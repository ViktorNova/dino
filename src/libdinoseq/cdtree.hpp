#ifndef CDTREE_HPP
#define CDTREE_HPP

#include <cmath>


using namespace std;


/** This class represents a node in a CDTree. It is used by CDTree and should
    not be used directly. */
template <class T>
class CDTreeNode {
public:
  
  CDTreeNode(unsigned int children = 16, unsigned int depth = 5);
  ~CDTreeNode();
  
  bool set(unsigned int i, const T& data);
  const T& get(unsigned int i) const;
  bool fill_start(unsigned int i, const T& data, T& old_data);
  bool fill(const T&data, const T& old_data);
  unsigned int count_nodes() const;
  
private:
  
  bool is_prunable(const T& data) const;
  
  unsigned int m_depth;
  unsigned int m_size;
  unsigned int m_segmentsize;
  CDTreeNode<T>* m_children;
  T* m_data;
};


template <class T>
unsigned int CDTreeNode<T>::count_nodes() const {
  unsigned int result = 1;
  if (m_children) {
    for (unsigned int i = 0; i < m_size; ++i)
      result += m_children[i].count_nodes();
  }
  return result;
}


template <class T>
CDTreeNode<T>::CDTreeNode(unsigned int children, unsigned int depth)
  : m_depth(depth), 
    m_size(children), 
    m_segmentsize(int(pow(float(children), int(depth - 1)))),
    m_children(NULL),
    m_data(new T) {
  
}


template <class T>
CDTreeNode<T>::~CDTreeNode() {
  if (m_data)
    delete m_data;
  if (m_children)
    delete [] m_children;
}


template <class T>
bool CDTreeNode<T>::set(unsigned int i, const T& data) {
  assert(!(m_data && m_children));

  // if we're at a leaf, just set the data value
  if (m_depth == 0) {
    *m_data = data;
    return true;
  }
  
  // if we're not at a leaf and we don't have any children, initialise them
  if (m_data) {
    if (data == *m_data)
      return true;
    delete m_data;
    m_data = NULL;
    m_children = new CDTreeNode<T>[m_size];
    for (unsigned int i = 0; i < m_size; ++i)
      m_children[i] = CDTreeNode<T>(m_size, m_depth - 1);
  }
  
  // set the data value, check if we can prune the tree
  //m_children[i / m_segmentsize].set(i % m_segmentsize, data);
  if (m_children[i / m_segmentsize].set(i % m_segmentsize, data)) {
    bool prunable = true;
    for (unsigned int j = 0; j < m_size && prunable; ++j) {
      if (j != i / m_segmentsize) {
	if (!(prunable = m_children[j].is_prunable(data)))
	  return false;
      }
    }
    m_data = new T;
    *m_data = data;
    delete [] m_children;
    m_children = NULL;
    return true;
  }

  return false;
}


template <class T>
const T& CDTreeNode<T>::get(unsigned int i) const {
  assert(m_data || m_children);
  if (m_data)
    return *m_data;
  return m_children[i / m_segmentsize].get(i % m_segmentsize);
}


template <class T>
bool CDTreeNode<T>::fill_start(unsigned int i, const T& data, T& old_data) {
  if (m_depth == 0) {
    if (*m_data == data)
      return false;
    old_data = *m_data;
    *m_data = data;
    return true;
  }
  
  if (m_data) {
    if (*m_data == data) {
      old_data = *m_data;
      return true;
    }
    delete m_data;
    m_data = NULL;
    m_children = new CDTreeNode<T>[m_size];
    for (unsigned int i = 0; i < m_size; ++i)
      m_children[i] = CDTreeNode<T>(m_size, m_depth - 1);
  }
  
  unsigned int j = i / m_segmentsize;
  bool not_done = m_children[j].fill_start(i % m_segmentsize, 
					   data, old_data);
  for (++j; j < m_size && not_done; ++j) {
    not_done = m_children[j].fill(data, old_data);
  }
  
  return not_done;
}


template <class T>
bool CDTreeNode<T>::fill(const T&data, const T& old_data) {
  if (m_depth == 0) {
    if (*m_data == old_data) {
      *m_data = data;
      return true;
    }
    else
      return false;
  }
  
  if (m_data) {
    if (*m_data == old_data) {
      *m_data = data;
      return true;
    }
    delete m_data;
    m_data = NULL;
    m_children = new CDTreeNode<T>[m_size];
    for (unsigned int i = 0; i < m_size; ++i)
      m_children[i] = CDTreeNode<T>(m_size, m_depth - 1);
  }
  
  bool not_done = true;
  for (unsigned int j = 0; j < m_size && not_done; ++j) {
    not_done = m_children[j].fill(data, old_data);
  }
  
  return not_done;
}


template <class T>
bool CDTreeNode<T>::is_prunable(const T& data) const {
  if (m_data) {
    if (*m_data == data)
      return true;
    return false;
  }
  
  for (unsigned int i = 0; i < m_size; ++i) {
    if (!m_children[i].is_prunable(data))
      return false;
  }
  return true;
}


/** This is a data structure that has constant time random read access to
    its elements, but uses much less memory for sparse lists than a
    vector would do. */
template <class T>
class CDTree {
public:
  
  /** This constructor creates a new CDTree object with room for @c size
      elements. */
  CDTree(unsigned long size = 20, 
	 unsigned int children = 16, unsigned int depth = 6);
  
  /** This sets the ith element to @c data. */
  void set(unsigned int i, const T& data);
  /** This returns the ith element. */
  const T& get(unsigned int i) const;
  /** This function fills the interval [i, K) with the value @c data, 
      where K is the index of the first element that is not equal to element
      i. */
  void fill(unsigned int i, const T& data);
  
  unsigned int count_nodes() const;
  
private:
  
  unsigned long m_size;
  unsigned int m_segmentsize;
  CDTreeNode<T>* m_children;
};


template <class T>
CDTree<T>::CDTree(unsigned long size, 
		  unsigned int children, unsigned int depth)
  : m_size(size), 
    m_segmentsize(int(pow(float(children), int(depth - 1)))),
    m_children(new CDTreeNode<T>[m_size / m_segmentsize + 1]) {
  
  assert(depth != 0);
  
  for (unsigned int i = 0; i < m_size / m_segmentsize + 1; ++i)
    m_children[i] = CDTreeNode<T>(children, depth - 1);
}
  

template <class T>
void CDTree<T>::set(unsigned int i, const T& data) {
  T tmp;
  m_children[i / m_segmentsize].set(i % m_segmentsize, data);
}


template <class T>
const T& CDTree<T>::get(unsigned int i) const {
  assert(i < m_size);
  return m_children[i / m_segmentsize].get(i % m_segmentsize);
}


template <class T>
void CDTree<T>::fill(unsigned int i, const T& data) {
  int j = i / m_segmentsize;
  T old_data;
  bool not_done = m_children[j].fill_start(i % m_segmentsize, data, old_data);
  for (++j; (j * m_segmentsize < m_size) && not_done; ++j)
    not_done = m_children[j].fill(data, old_data);
}


template <class T>
unsigned int CDTree<T>::count_nodes() const {
  unsigned int result = 0;
  for (unsigned int i = 0; i * m_segmentsize < m_size; ++i)
    result += m_children[i].count_nodes();
  return result;
}


#endif
