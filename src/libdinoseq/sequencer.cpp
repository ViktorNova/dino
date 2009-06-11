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

#include <algorithm>
#include <stdexcept>
#include <string>

#include "sequencer.hpp"


using namespace std;


namespace Dino {
  

  Sequencer::Sequencer(shared_ptr<TempoMap const> tmap)
    : m_tmap(tmap ? tmap : throw domain_error("Invalid TempoMap pointer!")) {

  }
  
  
  Sequencer::Iterator 
  Sequencer::add_sequencable(shared_ptr<Sequencable const> sqbl) {
    if (!sqbl)
      throw domain_error("Invalid Sequencable pointer!");
    return Iterator(m_sqbls.insert(m_sqbls.end(), 
				   make_pair(sqbl, shared_ptr<Instrument>())));
  }
  
  
  shared_ptr<Instrument> Sequencer::get_instrument(Iterator iter) {
    return iter.base()->second;
  }
  
  
  shared_ptr<Instrument const> 
  Sequencer::get_instrument(Iterator iter) const {
    return shared_ptr<Instrument const>(iter.base()->second);
  }
  
  
  shared_ptr<TempoMap const> Sequencer::get_tempomap() const {
    return m_tmap;
  }
  
    
  Sequencer::Iterator 
  Sequencer::sqbl_begin() {
    return Iterator(m_sqbls.begin());
  }
  
  Sequencer::Iterator 
  Sequencer::sqbl_end() {
    return Iterator(m_sqbls.end());
  }
  
  
  Sequencer::Iterator 
  Sequencer::sqbl_find(shared_ptr<Sequencable const> match) {
    return Iterator(m_sqbls.find(match));
  }
  
  
  Sequencer::ConstIterator 
  Sequencer::sqbl_begin() const {
    return ConstIterator(m_sqbls.begin());
  }
  
  
  Sequencer::ConstIterator 
  Sequencer::sqbl_end() const {
    return ConstIterator(m_sqbls.end());
  }
  
  
  Sequencer::ConstIterator 
  Sequencer::sqbl_find(shared_ptr<Sequencable const> match) const {
    return ConstIterator(m_sqbls.find(match));
  }
  
  
  void Sequencer::remove_sequencable(Iterator iter) {
    m_sqbls.erase(iter.base());
  }
  
  
  void Sequencer::set_instrument(Iterator iter, 
				 shared_ptr<Instrument> instr) {
    iter.base()->second = instr;
  }
  
  
  void Sequencer::set_tempomap(shared_ptr<TempoMap const> tmap) {
    if (!tmap)
      throw domain_error("Invalid TempoMap pointer!");
    m_tmap = tmap;
  }
    
  
  void Sequencer::play() {
    throw logic_error(string(__PRETTY_FUNCTION__) + " is not implemented!");
  }
    
  
  void Sequencer::stop() {
    throw logic_error(string(__PRETTY_FUNCTION__) + " is not implemented!");
  }
    
  
  void Sequencer::go_to_time(const SongTime& st) {
    throw logic_error(string(__PRETTY_FUNCTION__) + " is not implemented!");
  }
  
  
}
