/*****************************************************************************
    libdinoseq - a library for MIDI sequencing
    Copyright (C) 2009  Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef GETFIRST_HPP
#define GETFIRST_HPP



namespace Dino {
  
  
  /** This is a functor type template that returns the @c first member of a
      pair when called. It can be used to transform a map or multimap iterator
      into a set-like iterator over the keys using boost::transform_iterator.
  */
  template <typename K, typename V>
  struct GetFirst {
    typedef K result_type;
    K& operator()(std::pair<K, V>& p) const { return p.first; }
    const K& operator()(const std::pair<K, V>& p) const { return p.first; }
  };


}


#endif
