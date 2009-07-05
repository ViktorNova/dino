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

#ifndef COPY_CONST_HPP
#define COPY_CONST_HPP


namespace Dino {


  /** A struct template that can be used to determine whether two types are the
      same. We could use the template with the same name in Boost, but there's
      no need to add that dependency for a small thing like this.
      
      @c is_same<S, T>::value is @c true if and only if @c S and @c T are the
      same type. 
  
      @ingroup meta 
  */
  template <typename S, typename T>
  struct is_same {
    
    /** This is true if @c S and c T are the same type. */
    static const bool value = false;

  };
  
  
  /** A specialisation of is_same. 
      
      @ingroup meta
  */
  template <typename T>
  struct is_same<T, T> {

    /** This is true if @c S and c T are the same type. */
    static const bool value = true;

  };
  
  
  /** A struct template that defines a new type with or without a @c const
      qualifier.
      
      @c const_if<T, B>::type is @c T @c const if @c B is @c true and @c T if
      @c B is @c false. 
      
      @ingroup meta
  */
  template <typename T, bool B>
  struct const_if { 
    
    /** This is @c T @c const if @c B is @c true, otherwise it's just @c T. */
    typedef T type; 

  };
  
  
  /** A specialisation of const_if.       

      @ingroup meta
  */
  template <typename T> 
  struct const_if<T, true> {

    /** This is @c T @c const if @c B is @c true, otherwise it's just @c T. */
    typedef T const type;

  };
  
  
  /** A struct template that defines a new type with or without a @c const
      qualifier.
      
      @c copy_const<S, T>::type is @c S @c const if @c T is a @c const type and
      @c S if it is not. 
      
      @ingroup meta
  */
  template <typename S, typename T> 
  struct copy_const { 
    
    /** This is @c T @c const if @c S is a @c const type, otherwise it's just
	@c T. */
    typedef typename const_if<T, is_same<S, S const>::value>::type type;

  };
  
  
}


#endif
