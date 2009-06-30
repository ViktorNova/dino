/*****************************************************************************
    libdinoseq_test - unit test module for libdinoseq
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

#include <limits>
#include <sstream>

#include <boost/test/unit_test.hpp>

#include "eventbuffer.hpp"
#include "ostreambuffer.hpp"
#include "sequencable.hpp"
#include "sequencer.hpp"
#include "tempomap.hpp"


using namespace Dino;
using namespace std;


BOOST_AUTO_TEST_SUITE(SequencerTest)


BOOST_AUTO_TEST_CASE(constructor) {
  BOOST_CHECK_NO_THROW(Sequencer seq());
}


class PhonySequencable : public Sequencable {
public:
  PhonySequencable() : Sequencable("Phony") {}
  bool sequence(Position&, SongTime const&, EventBuffer&) const { 
    return true; 
  }
};


BOOST_AUTO_TEST_CASE(add_remove_sequencable) {
  auto sqbl1 = make_shared<PhonySequencable>();
  auto sqbl2 = make_shared<PhonySequencable>();
  Sequencer seq;
  
  BOOST_CHECK_THROW(seq.add_sequencable(shared_ptr<PhonySequencable>()), 
		    domain_error);
  BOOST_CHECK(seq.sqbl_begin() == seq.sqbl_end());
  
  seq.add_sequencable(sqbl1);
  seq.add_sequencable(sqbl2);
  seq.add_sequencable(sqbl1);
  
  BOOST_CHECK_EQUAL(distance(seq.sqbl_begin(), seq.sqbl_end()), 3);
  
  BOOST_CHECK(sqbl1 == *seq.sqbl_begin());
  BOOST_CHECK(sqbl2 == *(++seq.sqbl_begin()));
  BOOST_CHECK(sqbl1 == *(++++seq.sqbl_begin()));
  
  seq.remove_sequencable(seq.sqbl_find(sqbl1));
  
  BOOST_CHECK_EQUAL(distance(seq.sqbl_begin(), seq.sqbl_end()), 2);
  
  BOOST_CHECK(seq.sqbl_find(sqbl1) != seq.sqbl_end());
}


class PhonyEventBuffer : public EventBuffer {
public:
  bool write_event(SongTime const&, size_t, unsigned char const*) { 
    return false; 
  }
};


BOOST_AUTO_TEST_CASE(get_set_event_buffer) {
  auto sqbl = make_shared<PhonySequencable>();
  auto buf = make_shared<PhonyEventBuffer>();
  Sequencer seq;
  
  seq.add_sequencable(sqbl);
  
  BOOST_CHECK_EQUAL(seq.get_event_buffer(seq.sqbl_find(sqbl)),
		    shared_ptr<EventBuffer>());

  seq.set_event_buffer(seq.sqbl_find(sqbl), buf);
  
  BOOST_CHECK_EQUAL(seq.get_event_buffer(seq.sqbl_find(sqbl)), buf);
  
  seq.set_event_buffer(seq.sqbl_find(sqbl), shared_ptr<EventBuffer>());
  
  BOOST_CHECK_EQUAL(seq.get_event_buffer(seq.sqbl_find(sqbl)),
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



BOOST_AUTO_TEST_CASE(run) {
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
    "0:00000000: 00\n"
    "1:00000000: 01\n"
    "2:00000000: 02\n"
    "3:00000000: 03\n"
    "4:00000000: 04\n"
    "5:00000000: 05\n"
    "0:00000000: 00\n"
    "1:00000000: 01\n"
    "2:00000000: 02\n"
    "3:00000000: 03\n"
    "4:00000000: 04\n"
    "5:00000000: 05\n";
  
  BOOST_CHECK(os1.str() == expected_result);
  
  BOOST_CHECK(os2.str() == expected_result);
}


BOOST_AUTO_TEST_SUITE_END()
