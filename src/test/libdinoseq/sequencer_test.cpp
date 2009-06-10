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
  shared_ptr<Sequencable> sqbl1 = make_shared<Sequencable>();
  shared_ptr<Sequencable> sqbl2 = make_shared<Sequencable>();
  Sequencer seq(make_shared<TempoMap>());
  
  BOOST_CHECK_THROW(seq.add_sequencable(shared_ptr<Sequencable>()), 
		    domain_error);
  BOOST_CHECK(seq.sqbl_begin() == seq.sqbl_end());
  
  seq.add_sequencable(sqbl1);
  seq.add_sequencable(sqbl2);
  seq.add_sequencable(sqbl1);
  
  BOOST_CHECK_EQUAL(distance(seq.sqbl_begin(), seq.sqbl_end()), 3);
  
  shared_ptr<Sequencable> min = sqbl1 < sqbl2 ? sqbl1 : sqbl2;
  shared_ptr<Sequencable> max = sqbl1 < sqbl2 ? sqbl2 : sqbl1;
  
  BOOST_CHECK_EQUAL(min, *seq.sqbl_begin());
  BOOST_CHECK_EQUAL(max, *(++++seq.sqbl_begin()));
  
  seq.remove_sequencable(seq.sqbl_find(sqbl1));
  
  BOOST_CHECK_EQUAL(distance(seq.sqbl_begin(), seq.sqbl_end()), 2);
  
  BOOST_CHECK(seq.sqbl_find(sqbl1) != seq.sqbl_end());
}


BOOST_AUTO_TEST_CASE(get_set_instrument) {
  shared_ptr<Sequencable> sqbl = make_shared<Sequencable>();
  shared_ptr<Instrument> instr = make_shared<Instrument>();
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
  shared_ptr<TempoMap> tmap = make_shared<TempoMap>();
  shared_ptr<TempoMap> tmap2 = make_shared<TempoMap>();
  shared_ptr<TempoMap> tmap3;
  Sequencer seq(tmap);
  
  seq.set_tempomap(tmap2);
  
  BOOST_CHECK_EQUAL(seq.get_tempomap(), tmap2);
  
  BOOST_CHECK_THROW(seq.set_tempomap(tmap3), domain_error);
  
  BOOST_CHECK_EQUAL(seq.get_tempomap(), tmap2);
}


BOOST_AUTO_TEST_SUITE_END()
