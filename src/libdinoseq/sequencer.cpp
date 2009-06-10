#include <algorithm>
#include <stdexcept>
#include <string>

#include "sequencer.hpp"


using namespace std;


namespace Dino {
  

  Sequencer::Sequencer(shared_ptr<TempoMap const> tmap)
    : m_tmap(tmap) {
    if (!tmap)
      throw domain_error("Invalid TempoMap pointer!");
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
