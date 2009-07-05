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

#include <memory>
#include <stdexcept>
#include <utility>

#include <boost/iterator/transform_iterator.hpp>

#include "linkedlist.hpp"
#include "sequencable.hpp"
#include "songtime.hpp"


namespace Dino {
  
  
  class EventBuffer;
  
  
  /** This is the sequencer engine. It holds references to a collection
      of Sequencable objects and EventBuffer objects, and sequences data from
      the former into the latter. */
  class Sequencer {
    
    struct SeqData {
      SeqData() throw() {}
      SeqData(SeqData&& sd) throw()
	: seq(sd.seq), pos(std::move(sd.pos)), buf(sd.buf) {}
      SeqData(SeqData const&) = delete;
      std::shared_ptr<Sequencable const> seq;
      std::unique_ptr<Sequencable::Position> pos;
      std::shared_ptr<EventBuffer> buf;
    };
    
    struct GetSqbl {
      std::shared_ptr<Sequencable const> const& 
      operator()(SeqData const& wrp) const throw() {
	return wrp.seq;
      }
    };
    
  public:
    
    /** The iterator type for iterating over Sequencables. */
    typedef boost::transform_iterator<GetSqbl,
				      LinkedList<SeqData>::Iterator,
				      std::shared_ptr<Sequencable const>const&>
    Iterator;

    /** The const iterator type for iterating over Sequencables. */
    typedef boost::transform_iterator<GetSqbl,
				      LinkedList<SeqData>::ConstIterator, 
				      std::shared_ptr<Sequencable const> const&>
  ConstIterator;
    
    Sequencer();

    /** Return the event buffer that the Sequencable that @c iter refers to 
	will be sequenced to. */
    std::shared_ptr<EventBuffer> get_event_buffer(Iterator iter) throw();
    
    /** Return the EventBuffer that the Sequencable that @c iter refers to will
	be sequenced to, const version. */
    std::shared_ptr<EventBuffer const> get_event_buffer(Iterator iter) const
      throw();
    
    /** Return an Iterator to the first Sequencable that is sequenced by
	this object. */
    Iterator sqbl_begin() throw();
    
    /** Return an Iterator to the end of the list of Sequencables that are
	sequenced by this object. */
    Iterator sqbl_end() throw();
    
    /** If @c match is in the list of Sequencables that are sequenced by
	this object, return an Iterator to the first occurance of it.
	Otherwise, return sqbl_end(). */
    Iterator sqbl_find(std::shared_ptr<Sequencable const> match) throw();
    
    /** Return a ConstIterator to the first Sequencable that is sequenced by
	this object. */
    ConstIterator sqbl_begin() const throw();
    
    /** Return a ConstIterator to the end of the list of Sequencables that are
	sequenced by this object. */
    ConstIterator sqbl_end() const throw();
    
    /** If @c match is in the list of Sequencables that are sequenced by
	this object, return a ConstIterator to the first occurance of it.
	Otherwise, return sqbl_end(). */
    ConstIterator 
    sqbl_find(std::shared_ptr<Sequencable const> match) const throw();
    
    /** Add an object to the list of sequenced objects. */
    Iterator add_sequencable(std::shared_ptr<Sequencable const> sqbl) 
      throw(std::bad_alloc, std::overflow_error);
    
    /** Remove the reference to the Sequencable that @c iter refers to from
	the list, which means that it will not be sequenced any more. */
    void remove_sequencable(Iterator iter) throw(std::overflow_error);
    
    /** Set the buffer that the Sequencable that @c iter refers to will be
	sequenced to. */
    void set_event_buffer(Iterator iter, std::shared_ptr<EventBuffer> instr)
      throw();
    
    /** This is the function that does the actual sequencing. */
    void run(SongTime const& from, SongTime const& to);
    
  private:
    
    LinkedList<SeqData> m_sqbls;
    
    SongTime m_next_start;
    
  };


}


#endif
