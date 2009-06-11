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

#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include <map>
#include <memory>

#include <boost/iterator/transform_iterator.hpp>

#include "getfirst.hpp"


namespace Dino {
  

  class Instrument;
  class Sequencable;
  class SongTime;
  class TempoMap;
  
  
  /** This is the sequencer engine. It holds references to a collection
      of Sequencable objects and Instrument objects, and sequences data from
      the former into the latter. */
  class Sequencer {
  public:
    
    /** The type of the data structure that contains the pointers to the
	Sequencable and Instrument objects. No objects of this type will be
	exposed in the public interface to the Sequencer class, it's just here
	to make the typedefs for Iterator and ConstIterator easier to read. */
    typedef std::multimap<std::shared_ptr<Sequencable const>,
			  std::shared_ptr<Instrument>> SequencableMap;
    
    /** The iterator type for iterating over Sequencables. */
    typedef boost::transform_iterator<GetFirst<SequencableMap::key_type,
					       SequencableMap::mapped_type>,
				      SequencableMap::iterator> Iterator;

    /** The const iterator type for iterating over Sequencables. */
    typedef boost::transform_iterator<GetFirst<SequencableMap::key_type,
					       SequencableMap::mapped_type>,
				      SequencableMap::const_iterator> 
				      ConstIterator;
    
    /** The TempoMap pointed to by tmap will be used to map between frame time
	and song time. */
    Sequencer(std::shared_ptr<TempoMap const> tmap);

    /** Return the instrument that the Sequencable that @iter refers to will be
	sequenced to. */
    std::shared_ptr<Instrument> get_instrument(Iterator iter);
    
    /** Return the instrument that the Sequencable that @iter refers to will be
	sequenced to, const version. */
    std::shared_ptr<Instrument const> get_instrument(Iterator iter) const;
    
    /** Return the current TempoMap. */
    std::shared_ptr<TempoMap const> get_tempomap() const;
    
    Iterator sqbl_begin();
    
    Iterator sqbl_end();
    
    Iterator sqbl_find(std::shared_ptr<Sequencable const> match);
    
    ConstIterator sqbl_begin() const;
    
    ConstIterator sqbl_end() const;
    
    ConstIterator sqbl_find(std::shared_ptr<Sequencable const> match) const;
    
    /** Add an object to the list of sequenced objects. */
    Iterator add_sequencable(std::shared_ptr<Sequencable const> sqbl);
    
    /** Remove the reference to the Sequencable that @c iter refers to from
	the list, which means that it will not be sequenced any more. */
    void remove_sequencable(Iterator iter);
    
    /** Set the instrument that the Sequencable that @iter refers to will be
	sequenced to. */
    void set_instrument(Iterator iter, std::shared_ptr<Instrument> instr);

    /** The TempoMap pointed to by tmap will be used to map between frame time
	and song time. */
    void set_tempomap(std::shared_ptr<TempoMap const> tmap);
    
    /** Start playing at the current time. */
    void play();
    
    /** Stop playing, stay at the current time. */
    void stop();
    
    /** Go to the given time, don't change the playing/paused state. */
    void go_to_time(const SongTime& st);
    
  private:
    
    std::shared_ptr<TempoMap const> m_tmap;
    
    SequencableMap m_sqbls;
    
  };


}


#endif
