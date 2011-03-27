/*****************************************************************************
    libdinoseq_test - unit test module for libdinoseq
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

#include <limits>
#include <sstream>

#include "dtest.hpp"
#include "eventbuffer.hpp"
#include "ostreambuffer.hpp"
#include "sequencable.hpp"
#include "sequencer.hpp"
#include "tempomap.hpp"


using namespace Dino;
using namespace std;


namespace SequencerTest {


  void dtest_constructor() {
    DTEST_NOTHROW(Sequencer seq());
  }
  
  
  class PhonySequencable : public Sequencable {
  public:
    PhonySequencable() : Sequencable("Phony") {}
    bool sequence(Position&, SongTime const&, EventBuffer&) const { 
      return true; 
    }
  };
  
  
  void dtest_add_remove_sequencable() {
    auto sqbl1 = make_shared<PhonySequencable>();
    auto sqbl2 = make_shared<PhonySequencable>();
    Sequencer seq;
  
    DTEST_THROW_TYPE(seq.add_sequencable(shared_ptr<PhonySequencable>()), 
		     std::invalid_argument);
    DTEST_TRUE(seq.sqbl_begin() == seq.sqbl_end());
  
    seq.add_sequencable(sqbl1);
    seq.add_sequencable(sqbl2);
    seq.add_sequencable(sqbl1);
  
    DTEST_TRUE(distance(seq.sqbl_begin(), seq.sqbl_end()) == 3);
  
    DTEST_TRUE(sqbl1 == *seq.sqbl_begin());
    DTEST_TRUE(sqbl2 == *(++seq.sqbl_begin()));
    DTEST_TRUE(sqbl1 == *(++++seq.sqbl_begin()));
  
    seq.remove_sequencable(seq.sqbl_find(sqbl1));
  
    DTEST_TRUE(distance(seq.sqbl_begin(), seq.sqbl_end()) == 2);
  
    DTEST_TRUE(seq.sqbl_find(sqbl1) != seq.sqbl_end());
  }


  class PhonyEventBuffer : public EventBuffer {
  public:
    bool write_event(SongTime const&, size_t, unsigned char const*) { 
      return false; 
    }
  };


  void dtest_get_set_event_buffer() {
    auto sqbl = make_shared<PhonySequencable>();
    auto buf = make_shared<PhonyEventBuffer>();
    Sequencer seq;
  
    seq.add_sequencable(sqbl);
  
    DTEST_TRUE(seq.get_event_buffer(seq.sqbl_find(sqbl)) ==
	       shared_ptr<EventBuffer>());

    seq.set_event_buffer(seq.sqbl_find(sqbl), buf);
  
    DTEST_TRUE(seq.get_event_buffer(seq.sqbl_find(sqbl)) == buf);
  
    seq.set_event_buffer(seq.sqbl_find(sqbl), shared_ptr<EventBuffer>());
  
    DTEST_TRUE(seq.get_event_buffer(seq.sqbl_find(sqbl)) ==
	       shared_ptr<EventBuffer>());
  }


  class BeatSequence : public Sequencable {
  public:
  
    BeatSequence() : Sequencable("foo") { }
  
    bool sequence(Sequencable::Position& pos, 
		  SongTime const& to, EventBuffer& buf) const {
      SongTime::Beat b = pos.get_time().get_beat();
      if (pos.get_time().get_tick() > 0)
	++b;
      SongTime::Beat end = to.get_beat();
      if (to.get_tick() > 0)
	++end;
      for ( ; b < end; ++b) {
	unsigned char data = b % 0xFF;
	if (!buf.write_event(SongTime(b, 0), 1, &data)) {
	  update_position(pos, SongTime(b, 0));
	  return false;
	}
      }
    
      update_position(pos, to);
      return true;
    }
  
  };


  void dtest_run() {
    ostringstream os1;
    ostringstream os2;
    auto sqbl = make_shared<BeatSequence>();
    auto buf1 = make_shared<OStreamBuffer>(os1);
    auto buf2 = make_shared<OStreamBuffer>(os2);
    Sequencer seq;
  
    seq.set_event_buffer(seq.add_sequencable(sqbl), buf1);
    seq.set_event_buffer(seq.add_sequencable(sqbl), buf2);
  
    seq.run(SongTime(0, 0), SongTime(5, 0x838382));
    seq.run(SongTime(0, 0), SongTime(5, 0x838382));
    seq.run(SongTime(89, 1), SongTime(90, 0));
    seq.run(SongTime(90, 0), SongTime(90, 0));
  
    os1<<flush;
    os2<<flush;
  
    string expected_result = 
      "0:000000: 00\n"
      "1:000000: 01\n"
      "2:000000: 02\n"
      "3:000000: 03\n"
      "4:000000: 04\n"
      "5:000000: 05\n"
      "0:000000: 00\n"
      "1:000000: 01\n"
      "2:000000: 02\n"
      "3:000000: 03\n"
      "4:000000: 04\n"
      "5:000000: 05\n";
  
    DTEST_TRUE(os1.str() == expected_result);
  
    DTEST_TRUE(os2.str() == expected_result);
  }


}
