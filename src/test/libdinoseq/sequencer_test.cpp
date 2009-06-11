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

#include <boost/test/unit_test.hpp>

#include "instrument.hpp"
#include "sequencable.hpp"
#include "sequencer.hpp"
#include "tempomap.hpp"


using namespace Dino;
using namespace std;


BOOST_AUTO_TEST_SUITE(SequencerTest)


BOOST_AUTO_TEST_CASE(constructor) {
  BOOST_CHECK_NO_THROW(Sequencer seq(make_shared<TempoMap>()));
  BOOST_CHECK_THROW(Sequencer(shared_ptr<TempoMap>()), domain_error);
}


BOOST_AUTO_TEST_CASE(add_remove_sequencable) {
  auto sqbl1 = make_shared<Sequencable>();
  auto sqbl2 = make_shared<Sequencable>();
  Sequencer seq(make_shared<TempoMap>());
  
  BOOST_CHECK_THROW(seq.add_sequencable(shared_ptr<Sequencable>()), 
		    domain_error);
  BOOST_CHECK(seq.sqbl_begin() == seq.sqbl_end());
  
  seq.add_sequencable(sqbl1);
  seq.add_sequencable(sqbl2);
  seq.add_sequencable(sqbl1);
  
  BOOST_CHECK_EQUAL(distance(seq.sqbl_begin(), seq.sqbl_end()), 3);
  
  auto min = sqbl1 < sqbl2 ? sqbl1 : sqbl2;
  auto max = sqbl1 < sqbl2 ? sqbl2 : sqbl1;
  
  BOOST_CHECK_EQUAL(min, *seq.sqbl_begin());
  BOOST_CHECK_EQUAL(max, *(++++seq.sqbl_begin()));
  
  seq.remove_sequencable(seq.sqbl_find(sqbl1));
  
  BOOST_CHECK_EQUAL(distance(seq.sqbl_begin(), seq.sqbl_end()), 2);
  
  BOOST_CHECK(seq.sqbl_find(sqbl1) != seq.sqbl_end());
}


BOOST_AUTO_TEST_CASE(get_set_instrument) {
  auto sqbl = make_shared<Sequencable>();
  auto instr = make_shared<Instrument>();
  Sequencer seq(make_shared<TempoMap>());
  
  seq.add_sequencable(sqbl);
  
  BOOST_CHECK_EQUAL(seq.get_instrument(seq.sqbl_find(sqbl)),
		    shared_ptr<Instrument>());

  seq.set_instrument(seq.sqbl_find(sqbl), instr);
  
  BOOST_CHECK_EQUAL(seq.get_instrument(seq.sqbl_find(sqbl)), instr);
  
  seq.set_instrument(seq.sqbl_find(sqbl), shared_ptr<Instrument>());
  
  BOOST_CHECK_EQUAL(seq.get_instrument(seq.sqbl_find(sqbl)),
		    shared_ptr<Instrument>());
}


BOOST_AUTO_TEST_CASE(get_set_tempomap) {
  auto tmap = make_shared<TempoMap>();
  auto tmap2 = make_shared<TempoMap>();
  shared_ptr<TempoMap> tmap3;
  Sequencer seq(tmap);
  
  seq.set_tempomap(tmap2);
  
  BOOST_CHECK_EQUAL(seq.get_tempomap(), tmap2);
  
  BOOST_CHECK_THROW(seq.set_tempomap(tmap3), domain_error);
  
  BOOST_CHECK_EQUAL(seq.get_tempomap(), tmap2);
}


BOOST_AUTO_TEST_SUITE_END()
