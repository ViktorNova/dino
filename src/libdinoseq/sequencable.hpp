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

#ifndef SEQUENCABLE_HPP
#define SEQUENCABLE_HPP

#include <memory>
#include <new>
#include <string>

#include "songtime.hpp"


namespace Dino {
  
  
  class EventBuffer;
  
  
  /** The base class for sequencable data structures.
      It declares pure virtual member functions that all subclasses
      must implement in order to be sequencable by the Sequencer class.
      
      @ingroup mididata */
  class Sequencable {
  public:
    
    /** This class abstracts a position in a sequencable object.    
    	The Sequencer keeps one Position object for each Sequencable it
	plays as the only state needed to keep playing that object
	where it left off. 
    
	@ingroup mididata */
    class Position {
    public:
      
      /** A virtual destructor is needed to delete safely. */
      virtual ~Position();
      
      /** Copying is not allowed. */
      Position(Position const&) = delete;
      
      /** Assignment is not allowed. */
      Position& operator=(Position const&) = delete;
      
      /** Return the time this position is at. */
      SongTime const& get_time() const throw();
      
    protected:
    
      /** Create a new Position with the given time. */
      Position(SongTime const& st);
      
    private:
      
      friend class Sequencable;
      
      SongTime m_time;
      
    };
    
    
    /** Create a new Sequencable object with the given label and length. */
    Sequencable(std::string const label, SongTime const& length = SongTime());
    
    /** Create a new Position object for this sequencable.
	The Position will start at the offset given by @c st. This function
	is @b not realtime safe. */
    virtual std::unique_ptr<Position> 
    create_position(SongTime const& st) const;
    
    /** Update a Position object to a new time. 
	This function is realtime safe and can be called by the sequencer
	in an RT thread. */
    virtual void update_position(Position& pos, SongTime const& st) const;
    
    /** Write MIDI data from the Sequencable to an EventBuffer.
	The function returns @c false if it was not able to write all events
	in the range [@c pos, @c to) to @c buf. @c pos will be updated to point
	to the end of the range of events that were written.
	This function is realtime safe. */
    virtual bool sequence(Position& pos, SongTime const& to, 
			  EventBuffer& buf) const = 0;
    
    /** Returns the label of this Sequencable. */
    std::string const& get_label() const throw();
    
    /** Returns the length of this Sequencable. If there is no applicable
	length (e.g. if the Sequencable generates events on the fly) it should
	return a reference to a SongTime(-1, 0). */
    SongTime const& get_length() const throw();
    
    /** Set the label for this Sequencable. */
    void set_label(std::string const& label);
    
    /** Set the length of this Sequencable, if applicable. */
    void set_length(SongTime const& st);
    
  private:
    
    std::string m_label;
    
    SongTime m_length;
    
  };


}


#endif
