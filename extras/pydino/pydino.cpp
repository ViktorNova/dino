#include <boost/python.hpp>
#include <controller.hpp>
#include <controller_numbers.hpp>
#include <interpolatedevent.hpp>
#include <note.hpp>
#include <notecollection.hpp>
#include <pattern.hpp>
#include <patternselection.hpp>
#include <sequencer.hpp>
#include <song.hpp>
#include <tempomap.hpp>
#include <track.hpp>

#include "signalwrappers.hpp"


using namespace boost::python;
using namespace Dino;


// C++ workarounds for weird Boost.Python errors
namespace {
  
  std::string Controller_get_name(Controller& c) {
    return c.get_name();
  }
  
  std::string Pattern_get_name(Pattern& p) {
    return p.get_name();
  }
  
  Note& Pattern_find_note(Pattern&p, unsigned int step, int key) {
    return *p.find_note(step, key);
  }

  const Controller& Pattern_ctrls_find(Pattern&p, long param) {
    return *p.ctrls_find(param);
  }
  
  void Pattern_add_note(Pattern& p, unsigned step, int key, int velocity, 
			int length) {
    p.add_note(step, key, velocity, length);
  }
  
  PatternSelection Pattern_add_notes(Pattern& p, const NoteCollection& notes,
				     unsigned step, int key) {
    PatternSelection result(&p);
    p.add_notes(notes, step, key, &result);
    return result;
  }
  
  void Pattern_delete_note(Pattern& p, unsigned int step, unsigned char key) {
    Pattern::NoteIterator iter = p.find_note(step, key);
    if (iter != p.notes_end())
      p.delete_note(iter);
  }
  
  void Pattern_resize_note(Pattern& p, unsigned int step, unsigned char key,
			   int length) {
    Pattern::NoteIterator iter = p.find_note(step, key);
    if (iter != p.notes_end())
      p.resize_note(iter, length);
  }
  
  void Pattern_set_velocity(Pattern& p, unsigned int step, unsigned char key,
			    unsigned char vel) {
    Pattern::NoteIterator iter = p.find_note(step, key);
    if (iter != p.notes_end())
      p.set_velocity(iter, vel);
  }
  
  void Pattern_add_controller(Pattern& p, const std::string& name, long param,
			      int min, int max) {
    p.add_controller(name, param, min, max);
  }
  
  void Pattern_remove_controller(Pattern& p, long param) {
    Pattern::ControllerIterator iter = p.ctrls_find(param);
    if (iter != p.ctrls_end())
      p.remove_controller(iter);
  }
  
  void Pattern_add_cc(Pattern& p, long param, unsigned int step, int value) {
    Pattern::ControllerIterator iter = p.ctrls_find(param);
    if (iter != p.ctrls_end())
      p.add_cc(iter, step, value);
  }
  
  void Pattern_remove_cc(Pattern& p, long param, unsigned int step) {
    Pattern::ControllerIterator iter = p.ctrls_find(param);
    if (iter != p.ctrls_end())
      p.remove_cc(iter, step);
  }
  
  SWR1(Pattern, name_changed, std::string);
  SWR1(Pattern, length_changed, int);
  SWR1(Pattern, steps_changed, int);
  SWR1(Pattern, note_added, const Note&);
  SWR1(Pattern, note_changed, const Note&);
  SWR1(Pattern, note_removed, const Note&);
  SWR3(Pattern, cc_added, int, int, int);
  SWR3(Pattern, cc_changed, int, int, int);
  SWR2(Pattern, cc_removed, int, int);
  SWR1(Pattern, controller_added, int);
  SWR1(Pattern, controller_removed, int);
  
  Note* PatternSelection_find(PatternSelection& ps, 
			      unsigned int step, unsigned char key) {
    if (!ps.get_pattern())
      return 0;
    return &*ps.get_pattern()->find_note(step, key);
  }
  
  void PatternSelection_add_note(PatternSelection& ps, const Note& note) {
    Pattern* pat = ps.get_pattern();
    if (pat)
      return;
    Pattern::NoteIterator iter = pat->find_note(note.get_step(),note.get_key());
    if (iter != pat->notes_end())
      ps.add_note(iter);
  }
  
  void PatternSelection_remove_note(PatternSelection& ps, const Note& note) {
    Pattern* pat = ps.get_pattern();
    if (pat)
      return;
    Pattern::NoteIterator iter = pat->find_note(note.get_step(),note.get_key());
    if (iter != pat->notes_end())
      ps.remove_note(iter);
  }
  
  boost::python::list Sequencer_get_instruments(Sequencer& seq) {
    boost::python::list result;
    std::vector<Dino::Sequencer::InstrumentInfo> instrs = seq.get_instruments();
    for (unsigned i = 0; i < instrs.size(); ++i)
      result.append(instrs[i]);
    return result;
  }

  SWR1(Sequencer, beat_changed, int);
  SWR0(Sequencer, instruments_changed);
  
  std::string Song_get_title(Song& s) {
    return s.get_title();
  }
  
  std::string Song_get_author(Song& s) {
    return s.get_author();
  }
  
  std::string Song_get_info(Song& s) {
    return s.get_info();
  }
  
  Track& Song_add_track(Song& s, const std::string& name) {
    return *s.add_track(name);
  }
  
  void Song_remove_track(Song& s, int id) {
    Song::TrackIterator iter = s.tracks_find(id);
    if (iter != s.tracks_end())
      s.remove_track(iter);
  }
  
  void Song_add_tempo_change(Song& s, int beat, double bpm) {
    s.add_tempo_change(beat, bpm);
  }
  
  void Song_remove_tempo_change(Song& s, int beat) {
    Song::TempoIterator iter = s.tempo_find(beat);
    if (iter != s.tempo_end())
      s.remove_tempo_change(iter);
  }
  
  Track* Song_find_track(Song& s, int id) {
    Song::TrackIterator iter = s.tracks_find(id);
    if (iter == s.tracks_end())
      return 0;
    return &*iter;
  }
  
  SWR1(Song, title_changed, const std::string&);
  SWR1(Song, author_changed, const std::string&);
  SWR1(Song, info_changed, const std::string&);
  SWR1(Song, length_changed, int);
  SWR1(Song, track_added, int);
  SWR1(Song, track_removed, int);
  SWR0(Song, tempo_changed);
  
  
  std::string Track_get_name(Track& t) {
    return t.get_name();
  }
  
  Pattern& Track_add_pattern(Track& t, const std::string& name, 
			     int length, int steps) {
    return *t.add_pattern(name, length, steps);
  }
  
  Pattern* Track_duplicate_pattern(Track& t, int id) {
    Track::PatternIterator iter = t.pat_find(id);
    if (iter == t.pat_end())
      return 0;
    return &*t.duplicate_pattern(iter);
  }
  
  void Track_set_sequence_entry(Track& t, int beat, int pattern, 
				unsigned int length) {
    t.set_sequence_entry(beat, pattern, length);
  }
  
  void Track_set_seq_entry_length(Track& t, int beat, unsigned int length) {
    Track::SequenceIterator iter = t.seq_find(beat);
    if (iter != t.seq_end())
      t.set_seq_entry_length(iter, length);
  }
  
  void Track_remove_sequence_entry(Track& t, int beat) {
    Track::SequenceIterator iter = t.seq_find(beat);
    if (iter != t.seq_end())
      t.remove_sequence_entry(iter);
  }
  
  Pattern* Track_find_pattern(Track& t, int id) {
    Track::PatternIterator iter = t.pat_find(id);
    if (iter == t.pat_end())
      return 0;
    return &*iter;
  }
  
  const Track::SequenceEntry* Track_find_sequence_entry(Track& t, int beat) {
    Track::SequenceIterator iter = t.seq_find(beat);
    if (iter == t.seq_end())
      return 0;
    return &*iter;
  }
  
  SWR1(Track, name_changed, const std::string&);
  SWR1(Track, pattern_added, int);
  SWR1(Track, pattern_removed, int);
  SWR3(Track, sequence_entry_added, int, int, int);
  SWR3(Track, sequence_entry_changed, int, int, int);
  SWR1(Track, sequence_entry_removed, int);
  SWR1(Track, length_changed, int);

}


BOOST_PYTHON_MODULE(dino) {
  
  // The sigc::connection wrapper class
  class_<Connection> _Connection("Connection", no_init);
  _Connection.def("disconnect", &Connection::disconnect);
  
  // Define all classes first so we can use the converters for return values
  // and arguments
  class_<Controller> _Controller("Controller", 
				 init<const std::string, unsigned int,
				 long, int, int>());
  class_<InterpolatedEvent> _InterpolatedEvent("InterpolatedEvent",
					       init<int, int, unsigned, 
					       unsigned>());
  class_<Note> _Note("Note", no_init);
  class_<NoteCollection> _NoteCollection("NoteCollection", 
					 init<const PatternSelection&>());
  scope* NoteCollectionScope = new scope(_NoteCollection);
  class_<NoteCollection::NoteDescription> _NoteDescription("NoteDescription",
							   init<unsigned int,
							   unsigned int,
							   unsigned char,
							   unsigned char>());
  delete NoteCollectionScope;
  class_<Pattern, boost::noncopyable> _Pattern("Pattern", init<int, 
					       const std::string&, int, int>());
  class_<PatternSelection> _PatternSelection("PatternSelection",
					     init<Pattern*>());
  class_<Sequencer> _Sequencer("Sequencer", init<const std::string&, Song&>()
			       [with_custodian_and_ward<1, 3>()]);
  scope* SequencerScope = new scope(_Sequencer);
  class_<Sequencer::InstrumentInfo> _InstrumentInfo("InstrumentInfo",
						    init<const std::string&>());
  delete SequencerScope;
  class_<Song, boost::noncopyable> _Song("Song", init<>());
  scope* SongScope = new scope(_Song);
  class_<TempoMap::TempoChange, boost::noncopyable> _TempoChange("TempoChange",
								 no_init);
  delete SongScope;
  class_<Track, boost::noncopyable> _Track("Track", init<int, int, 
					   const std::string&>());
  scope* TrackScope = new scope(_Track);
  class_<Track::SequenceEntry> _SequenceEntry("SequenceEntry", no_init);
  
  // Global functions from controller_numbers.hpp
  def("make_invalid", &make_invalid);
  def("controller_is_set", &controller_is_set);
  def("make_cc", &make_cc);
  def("is_cc", &is_cc);
  def("cc_number", &cc_number);
  def("make_nrpn", &make_nrpn);
  def("is_nrpn", &is_nrpn);
  def("nrpn_number", &nrpn_number);
  def("make_pbend", &make_pbend);
  def("is_pbend", &is_pbend);
  
  // Dino::Controller
  _Controller.add_property("name", &Controller_get_name, 
				&Controller::set_name);
  _Controller.def("get_event", &Controller::get_event, 
		  return_internal_reference<1>());
  _Controller.add_property("min", &Controller::get_min);
  _Controller.add_property("max", &Controller::get_max);
  _Controller.add_property("param", &Controller::get_param);
  _Controller.add_property("size", &Controller::get_size);
  _Controller.def("add_point", &Controller::add_point);
  _Controller.def("remove_point", &Controller::remove_point);
  
  // Dino::InterpolatedEvent
  _InterpolatedEvent.add_property("step", &InterpolatedEvent::get_step,
				  &InterpolatedEvent::set_step);
  _InterpolatedEvent.add_property("length", &InterpolatedEvent::get_length,
				  &InterpolatedEvent::set_length);
  _InterpolatedEvent.add_property("start", &InterpolatedEvent::get_start,
				  &InterpolatedEvent::set_start);
  _InterpolatedEvent.add_property("end", &InterpolatedEvent::get_end,
				  &InterpolatedEvent::set_end);
  
  // Dino::Note
  _Note.add_property("length", &Note::get_length);
  _Note.add_property("key", &Note::get_key);
  _Note.add_property("velocity", &Note::get_velocity);
  _Note.add_property("step", &Note::get_step);
  
  // Dino::NoteCollection
  _NoteCollection.def(init<>());
  {
    NoteCollection::Iterator(NoteCollection::*begin)() = &NoteCollection::begin;
    NoteCollection::Iterator (NoteCollection::*end)() = &NoteCollection::end;
    _NoteCollection.add_property("notes", range(begin, end));
  }
  _NoteCollection.def("printall", &NoteCollection::printall);
  
  // Dino::NoteCollection::NoteDescription
  _NoteDescription.def_readwrite("start", 
				 &NoteCollection::NoteDescription::start);
  _NoteDescription.def_readwrite("length", 
				 &NoteCollection::NoteDescription::length);
  _NoteDescription.def_readwrite("key", &NoteCollection::NoteDescription::key);
  _NoteDescription.def_readwrite("velocity", 
				 &NoteCollection::NoteDescription::velocity);
  
  // Dino::Pattern
  _Pattern.def(init<int, const Pattern&>());
  _Pattern.add_property("id", &Pattern::get_id);
  _Pattern.add_property("name", &Pattern_get_name, &Pattern::set_name);
  _Pattern.add_property("length", &Pattern::get_length, &Pattern::set_length);
  _Pattern.add_property("steps", &Pattern::get_steps, &Pattern::set_steps);
  {
    Pattern::NoteIterator (Pattern::*begin)() const = &Pattern::notes_begin;
    Pattern::NoteIterator (Pattern::*end)() const = &Pattern::notes_end;
    _Pattern.add_property("notes", range(begin, end));
  }
  {
    Pattern::ControllerIterator(Pattern::*begin)()const = &Pattern::ctrls_begin;
    Pattern::ControllerIterator(Pattern::*end)()const = &Pattern::ctrls_end;
    _Pattern.add_property("controllers", begin, end);
  }
  _Pattern.def("find_note", &Pattern_find_note, return_internal_reference<1>());
  _Pattern.def("find_controller", &Pattern_ctrls_find, 
	       return_internal_reference<1>());
  _Pattern.def("add_note", &Pattern_add_note);
  _Pattern.def("add_notes", &Pattern_add_notes);
  _Pattern.def("delete_note", &Pattern_delete_note);
  _Pattern.def("resize_note", &Pattern_resize_note);
  _Pattern.def("set_velocity", &Pattern_set_velocity);
  _Pattern.def("add_controller", &Pattern_add_controller);
  _Pattern.def("remove_controller", &Pattern_remove_controller);
  _Pattern.def("add_cc", &Pattern_add_cc);
  _Pattern.def("remove_cc", &Pattern_remove_cc);
  _Pattern.def("signal_name_changed", &Pattern_signal_name_changed);
  _Pattern.def("signal_steps_changed", &Pattern_signal_steps_changed);
  _Pattern.def("signal_length_changed", &Pattern_signal_length_changed);
  _Pattern.def("signal_note_added", &Pattern_signal_note_added);
  _Pattern.def("signal_note_changed", &Pattern_signal_note_changed);
  _Pattern.def("signal_note_removed", &Pattern_signal_note_removed);
  _Pattern.def("signal_cc_added", &Pattern_signal_cc_added);
  _Pattern.def("signal_cc_changed", &Pattern_signal_cc_changed);
  _Pattern.def("signal_cc_removed", &Pattern_signal_cc_removed);
  _Pattern.def("signal_controller_added", &Pattern_signal_controller_added);
  _Pattern.def("signal_controller_removed", &Pattern_signal_controller_removed);
  
  // Dino::PatternSelection
  _PatternSelection.def(init<>());
  _PatternSelection.add_property("notes", range(&PatternSelection::begin, 
						&PatternSelection::end));
  _PatternSelection.def("find", &PatternSelection_find, 
			return_internal_reference<1>());
  {
    Pattern*(PatternSelection::*get_pattern)() = &PatternSelection::get_pattern;
    _PatternSelection.def("get_pattern", get_pattern, 
			  return_internal_reference<1>());
  }
  _PatternSelection.def("add_note", &PatternSelection_add_note);
  _PatternSelection.def("remove_note", &PatternSelection_remove_note);
  _PatternSelection.def("clear", &PatternSelection::clear);
  _PatternSelection.def("printall", &PatternSelection::printall);
  
  // Dino::Sequencer
  _Sequencer.def("play", &Sequencer::play);
  _Sequencer.def("stop", &Sequencer::stop);
  _Sequencer.def("go_to_beat", &Sequencer::go_to_beat);
  _Sequencer.def("is_valid", &Sequencer::is_valid);
  _Sequencer.add_property("instruments", &Sequencer_get_instruments);
  _Sequencer.def("set_instrument", &Sequencer::set_instrument);
  _Sequencer.def("reset_ports", &Sequencer::reset_ports);
  _Sequencer.def("signal_beat_changed", &Sequencer_signal_beat_changed);
  _Sequencer.def("signal_instruments_changed", 
		 &Sequencer_signal_instruments_changed);
  
  // Dino::Sequencer::InstrumentInfo
  _InstrumentInfo.def_readwrite("name", &Sequencer::InstrumentInfo::name);
  _InstrumentInfo.def_readwrite("connected", 
				&Sequencer::InstrumentInfo::connected);
  
  // Dino::Song
  _Song.add_property("title", &Song_get_title, &Song::set_title);
  _Song.add_property("author", &Song_get_author, &Song::set_author);
  _Song.add_property("info", &Song_get_info, &Song::set_info);
  _Song.add_property("length", &Song::get_length, &Song::set_length);
  _Song.def("get_number_of_tracks", &Song::get_number_of_tracks);
  _Song.def("add_track", &Song_add_track, return_internal_reference<1>());
  _Song.def("remove_track", &Song_remove_track);
  _Song.def("add_tempo_change", &Song_add_tempo_change);
  _Song.def("remove_tempo_change", &Song_remove_tempo_change);
  _Song.def("write_file", &Song::write_file);
  _Song.def("load_file", &Song::load_file);
  _Song.def("clear", &Song::clear);
  _Song.def("find_track", &Song_find_track, return_internal_reference<1>());
  {
    Song::TrackIterator (Song::*begin)() = &Song::tracks_begin;
    Song::TrackIterator (Song::*end)() = &Song::tracks_end;
    _Song.add_property("tracks", range<return_internal_reference<1> >(begin, 
								      end));
  }
  _Song.add_property("tempochanges", range<return_internal_reference<1> >
		     (&Song::tempo_begin, &Song::tempo_end));
  _Song.def("signal_title_changed", &Song_signal_title_changed);
  _Song.def("signal_author_changed", &Song_signal_author_changed);
  _Song.def("signal_info_changed", &Song_signal_info_changed);
  _Song.def("signal_length_changed", &Song_signal_length_changed);
  _Song.def("signal_track_added", &Song_signal_track_added);
  _Song.def("signal_track_removed", &Song_signal_track_removed);
  _Song.def("signal_tempo_changed", &Song_signal_tempo_changed);
  
  // Dino::TempoMap::TempoChange (moved to Dino::Song::TempoChange since we
  // don't expose Dino::TempoMap in the Python wrapper)
  _TempoChange.def_readonly("beat", &TempoMap::TempoChange::get_beat);
  _TempoChange.def_readonly("bpm", &TempoMap::TempoChange::get_bpm);
  
  // Dino::Track
  _Track.add_property("id", &Track::get_id);
  _Track.add_property("name", &Track_get_name, &Track::set_name);
  _Track.add_property("channel", &Track::get_channel, &Track::set_channel);
  _Track.add_property("length", &Track::get_length, &Track::set_length);
  _Track.def("add_pattern", &Track_add_pattern, return_internal_reference<1>());
  _Track.def("duplicate_pattern", &Track_duplicate_pattern,
	     return_internal_reference<1>());
  _Track.def("remove_pattern", &Track::remove_pattern);
  _Track.def("set_sequence_entry", &Track_set_sequence_entry);
  _Track.def("set_seq_entry_length", &Track_set_seq_entry_length);
  _Track.def("remove_sequence_entry", &Track_remove_sequence_entry);
  _Track.def("find_pattern", &Track_find_pattern, 
	     return_internal_reference<1>());
  _Track.def("find_sequence_entry", &Track_find_sequence_entry, 
	     return_internal_reference<1>());
  {
    Track::PatternIterator (Track::*begin)() = &Track::pat_begin;
    Track::PatternIterator (Track::*end)() = &Track::pat_end;
    _Track.add_property("patterns", range<return_internal_reference<1> >(begin,
									 end));
  }
  {
    Track::SequenceIterator (Track::*begin)() const = &Track::seq_begin;
    Track::SequenceIterator (Track::*end)() const = &Track::seq_end;
    _Track.add_property("sequence", range(begin, end));
  }
  _Track.def("signal_name_changed", &Track_signal_name_changed);
  _Track.def("signal_pattern_added", &Track_signal_pattern_added);
  _Track.def("signal_pattern_removed", &Track_signal_pattern_removed);
  _Track.def("signal_sequence_entry_added", &Track_signal_sequence_entry_added);
  _Track.def("signal_sequence_entry_changed", 
	     &Track_signal_sequence_entry_changed);
  _Track.def("signal_sequence_entry_removed", 
	     &Track_signal_sequence_entry_removed);
  _Track.def("signal_length_changed", 
	     &Track_signal_length_changed);
  
  // Dino::Track::SequenceEntry
  _SequenceEntry.add_property("pattern_id", 
			      &Track::SequenceEntry::get_pattern_id);
  _SequenceEntry.add_property("start", &Track::SequenceEntry::get_start);
  _SequenceEntry.add_property("length", &Track::SequenceEntry::get_length);
}
