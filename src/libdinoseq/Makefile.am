AM_CPPFLAGS = @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @DEBUG_CFLAGS@

lib_LTLIBRARIES = libdinoseq.la

libdinoseq_la_SOURCES = \
	cdtree.hpp \
	controller.cpp controller.hpp \
	controller_numbers.hpp \
	debug.hpp debug.cpp \
	deleter.hpp \
	interpolatedevent.cpp interpolatedevent.hpp \
	midibuffer.cpp midibuffer.hpp \
	note.cpp note.hpp \
	notecollection.cpp notecollection.hpp \
	noteevent.cpp noteevent.hpp \
	pattern.cpp pattern.hpp \
	patternselection.cpp patternselection.hpp \
	ringbuffer.hpp \
	sequencer.cpp sequencer.hpp \
	song.cpp song.hpp \
	tempomap.cpp tempomap.hpp \
	track.cpp track.hpp \
	xmlserialisable.hpp

pkginclude_HEADERS = \
	cdtree.hpp \
	controller.hpp \
	controller_numbers.hpp \
	debug.hpp \
	deleter.hpp \
	interpolatedevent.hpp \
	midibuffer.hpp \
	note.hpp \
	notecollection.hpp \
	noteevent.hpp \
	pattern.hpp \
	patternselection.hpp \
	ringbuffer.hpp \
	sequencer.hpp \
	song.hpp \
	tempomap.hpp \
	track.hpp \
	xmlserialisable.hpp

libdinoseq_la_LIBADD = @LIBXMLPP_LIBS@ @JACK_LIBS@
