#ifndef CDTREE_HPP
#define CDTREE_HPP

#include <cmath>
#include <cassert>

#include "deletable.hpp"


namespace Dino {

  /** This class represents a node in a CDTree. It is used by CDTree and should
      not be used directly. */
  template <class T>
  class CDTreeNode {
  public:
  
    CDTreeNode(unsigned int children = 16, unsigned int depth = 5);
    ~CDTreeNode();
  
    bool set(unsigned long i, const T& data);
    const T& get(unsigned long i) const;
    bool fill(unsigned long start, unsigned long end, const T&data);
    unsigned int count_nodes() const;
  
  private:
  
    CDTreeNode(const CDTreeNode&) { assert(0); }
    CDTreeNode& operator=(const CDTreeNode&) { assert(0); return *this; }
  
    bool is_prunable(const T& data) const;
    void delete_data();
    void delete_children();
  
    unsigned int m_depth;
    unsigned int m_size;
    unsigned int m_segmentsize;
    CDTreeNode<T>** m_children;
    T* m_data;
  };


  template <class T>
  unsigned int CDTreeNode<T>::count_nodes() const {
    unsigned int result = 1;
    if (m_children) {
      for (unsigned int i = 0; i < m_size; ++i)
	result += m_children[i]->count_nodes();
    }
    return result;
  }


  template <class T>
  CDTreeNode<T>::CDTreeNode(unsigned int children, unsigned int depth)
    : m_depth(depth), 
      m_size(children), 
      m_segmentsize(int(pow(float(children), int(depth - 1)))),
      m_children(NULL),
      m_data(new T()) {
  
  }


  template <class T>
  CDTreeNode<T>::~CDTreeNode() {
    if (m_data)
      delete_data();
    if (m_children)
      delete_children();
  }


  template <class T>
  bool CDTreeNode<T>::set(unsigned long i, const T& data) {
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
      m_children = new CDTreeNode<T>*[m_size];
      for (unsigned int i = 0; i < m_size; ++i) {
	m_children[i] = new CDTreeNode<T>(m_size, m_depth - 1);
	m_children[i]->fill(0, m_segmentsize, *m_data);
      }
      delete_data();
    }
  
    // set the data value, check if we can prune the tree
    if (m_children[i / m_segmentsize]->set(i % m_segmentsize, data)) {
      bool prunable = true;
      for (unsigned int j = 0; j < m_size && prunable; ++j) {
	if (j != i / m_segmentsize) {
	  if (!(prunable = m_children[j]->is_prunable(data)))
	    return false;
	}
      }
      m_data = new T(data);
      delete_children();
      return true;
    }

    return false;
  }


  template <class T>
  const T& CDTreeNode<T>::get(unsigned long i) const {
    assert(m_data || m_children);
    if (m_data)
      return *m_data;
    return m_children[i / m_segmentsize]->get(i % m_segmentsize);
  }


  template <class T>
  bool CDTreeNode<T>::fill(unsigned long start, unsigned long end, const T&data) {
    //cerr<<start<<", "<<end<<", m_size * m_segmentsize == "<<(m_size * m_segmentsize)<<endl;
    assert(start < end);
  
    // if we're at a leaf, just set the data
    if (m_depth == 0) {
      *m_data = data;
      return true;
    }
  
    // if we're at a pruned subtree
    if (m_data) {
    
      // if we're supposed to fill the whole subtree or the subtree value
      // is the same as the new value, just set the subtree value
      if ((start == 0 && end == m_size * m_segmentsize) || *m_data == data) {
	//cerr<<"whole subtree"<<endl;
	*m_data = data;
	return true;
      }
    
      // if we're only supposed to fill part of the tree we need children
      m_children = new CDTreeNode<T>*[m_size];
      for (unsigned int i = 0; i < m_size; ++i) {
	m_children[i] = new CDTreeNode<T>(m_size, m_depth - 1);
	m_children[i]->fill(0, m_segmentsize, *m_data);
      }
      delete_data();
    }
  
    // fill the children
    //cerr<<"all children"<<endl;
    unsigned long seg;
    unsigned long b = start % m_segmentsize;
    for (seg = start / m_segmentsize; seg < (end - 1) / m_segmentsize; ++seg) {
      m_children[seg]->fill(b, m_segmentsize, data);
      b = 0;
    }
    unsigned long e = end % m_segmentsize;
    e = (e == 0 ? m_segmentsize : e);
    m_children[seg]->fill(b, e, data);
  
    // check if we can prune
    for (unsigned int j = 0; j < m_size; ++j) {
      if (!m_children[j]->is_prunable(data))
	return false;
    }
    m_data = new T(data);
    delete_children();
  
    return true;
  }


  template <class T>
  bool CDTreeNode<T>::is_prunable(const T& data) const {
  
    // if we're at a leaf or pruned subtree, just check the data
    if (m_data) {
      if (*m_data == data)
	return true;
      return false;
    }
  
    // otherwise check recursively
    for (unsigned int i = 0; i < m_size; ++i) {
      if (!m_children[i]->is_prunable(data))
	return false;
    }
  
    return true;
  }


  template <class T>
  void CDTreeNode<T>::delete_data() {
    delete m_data;
    m_data = NULL;
  }


  template <class T>
  void CDTreeNode<T>::delete_children() {
    if (m_children) {
      for (unsigned int i = 0; i < m_size; ++i)
	delete m_children[i];
      delete [] m_children;
      m_children = NULL;
    }
  }



  /** This is a data structure that has constant time random read access to
      its elements, but uses much less memory for sparse lists than a
      vector would do. */
  template <class T>
  class CDTree : public Deletable {
  public:
  
    /** This constructor creates a new CDTree object with room for @c size
	elements. */
    CDTree(unsigned long size = 20, 
	   unsigned int children = 16, unsigned int depth = 6);
  
    /** This sets the ith element to @c data. */
    void set(unsigned long i, const T& data);
    /** This returns the ith element. */
    const T& get(unsigned long i) const;
    /** This function fills the interval [start, end) with the value @c data. */
    void fill(unsigned long start, unsigned long end, const T& data);
    /** This function returns the number of nodes in this CDTree<T> object. */
    unsigned int count_nodes() const;
  
  private:
  
    unsigned long m_size;
    unsigned int m_segmentsize;
    CDTreeNode<T>** m_children;
  };


  template <class T>
  CDTree<T>::CDTree(unsigned long size, 
		    unsigned int children, unsigned int depth)
    : m_size(size), 
      m_segmentsize(int(pow(float(children), int(depth - 1)))),
      m_children(new CDTreeNode<T>*[m_size / m_segmentsize + 1]) {
  
    assert(depth != 0);
  
    for (unsigned int i = 0; i < m_size / m_segmentsize + 1; ++i)
      m_children[i] = new CDTreeNode<T>(children, depth - 1);
  }
  

  template <class T>
  void CDTree<T>::set(unsigned long i, const T& data) {
    m_children[i / m_segmentsize]->set(i % m_segmentsize, data);
  }


  template <class T>
  const T& CDTree<T>::get(unsigned long i) const {
    assert(i < m_size);
    return m_children[i / m_segmentsize]->get(i % m_segmentsize);
  }


  template <class T>
  void CDTree<T>::fill(unsigned long start, unsigned long end, const T& data) {
    assert(start < end);
    unsigned long seg;
    unsigned long b = start % m_segmentsize;
    for (seg = start / m_segmentsize; seg < (end - 1) / m_segmentsize; ++seg) {
      m_children[seg]->fill(b, m_segmentsize, data);
      b = 0;
    }
    m_children[seg]->fill(b, end % m_segmentsize, data);
  }


  template <class T>
  unsigned int CDTree<T>::count_nodes() const {
    unsigned int result = 0;
    for (unsigned int i = 0; i * m_segmentsize < m_size; ++i)
      result += m_children[i]->count_nodes();
    return result;
  }


}


#endif
