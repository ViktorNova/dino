/*****************************************************************************
    libdinoseq - a library for MIDI sequencing
    Copyright (C) 2009  Lars Luthman <mail@larsluthman.net>
    
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

#include <algorithm>
#include <stdexcept>
#include <string>

#include "sequencer.hpp"


namespace Dino {
  

  using std::invalid_argument;
  using std::move;
  using std::shared_ptr;
  using std::bad_alloc;
  using std::overflow_error;
  

  Sequencer::Sequencer() {
  }
  
  
  Sequencer::Iterator 
  Sequencer::add_sequencable(shared_ptr<Sequencable const> sqbl) 
    throw(bad_alloc, overflow_error, invalid_argument) {
    if (!sqbl)
      throw invalid_argument("Invalid Sequencable pointer!");
    SeqData sd;
    sd.seq = sqbl;
    sd.pos = sqbl->create_position(SongTime());
    sd.buf = shared_ptr<EventBuffer>();
    return Iterator(m_sqbls.insert(m_sqbls.end(), move(sd)));
  }
  
  
  shared_ptr<EventBuffer> Sequencer::get_event_buffer(Iterator iter) throw() {
    return iter.base()->buf;
  }
  
  
  shared_ptr<EventBuffer const> 
  Sequencer::get_event_buffer(Iterator iter) const throw() {
    return shared_ptr<EventBuffer const>(iter.base()->buf);
  }
  
  
  Sequencer::Iterator Sequencer::sqbl_begin() throw() {
    return Iterator(m_sqbls.begin());
  }
  
  Sequencer::Iterator Sequencer::sqbl_end() throw() {
    return Iterator(m_sqbls.end());
  }
  
  
  Sequencer::Iterator 
  Sequencer::sqbl_find(shared_ptr<Sequencable const> match) throw() {
    for (auto i = m_sqbls.begin(); i != m_sqbls.end(); ++i) {
      if (i->seq == match)
	return Iterator(i);
    }
    return sqbl_end();
  }
  
  
  Sequencer::ConstIterator Sequencer::sqbl_begin() const throw() {
    return ConstIterator(m_sqbls.begin());
  }
  
  
  Sequencer::ConstIterator Sequencer::sqbl_end() const throw() {
    return ConstIterator(m_sqbls.end());
  }
  
  
  Sequencer::ConstIterator 
  Sequencer::sqbl_find(shared_ptr<Sequencable const> match) const throw() {
    for (auto i = m_sqbls.begin(); i != m_sqbls.end(); ++i) {
      if (i->seq == match)
	return ConstIterator(i);
    }
    return sqbl_end();
  }
  
  
  void Sequencer::remove_sequencable(Iterator iter) throw(overflow_error) {
    m_sqbls.erase(iter.base());
  }
  
  
  void Sequencer::set_event_buffer(Iterator iter, 
				   shared_ptr<EventBuffer> buf) throw() {
    iter.base()->buf = buf;
  }
  
  
  void Sequencer::run(SongTime const& from, SongTime const& to) {
    
    // let the list deallocate unused nodes we're no longer touching
    m_sqbls.reader_holds_no_iterator();
    
    // if the start time isn't the same as last call's end time, update
    if (m_next_start != from) {
      for (auto iter = m_sqbls.begin(); iter != m_sqbls.end(); ++iter)
	iter->seq->update_position(*iter->pos, from);
    }
    
    // sequence all the objects
    for (auto iter = m_sqbls.begin(); iter != m_sqbls.end(); ++iter)
      iter->seq->sequence(*iter->pos, to, *iter->buf);
    
    m_next_start = to;
  }
  
  
}
