AM_CPPFLAGS = @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @DEBUG_CFLAGS@

noinst_LTLIBRARIES = libdinoseq.la

libdinoseq_la_SOURCES = \
	controller_numbers.hpp \
	controller.cpp controller.hpp \
	deleter.hpp \
	interpolatedevent.cpp interpolatedevent.hpp \
	midibuffer.cpp midibuffer.hpp \
	note.cpp note.hpp \
	notecollection.cpp notecollection.hpp \
	noteevent.cpp noteevent.hpp \
	pattern.cpp pattern.hpp \
	patternselection.cpp patternselection.hpp \
	track.cpp track.hpp \
	song.cpp song.hpp \
	ringbuffer.hpp \
	sequencer.cpp sequencer.hpp \
	tempomap.cpp tempomap.hpp \
	cdtree.hpp \
	debug.hpp debug.cpp \
	xmlserialisable.hpp

libdinoseq_la_LIBADD = @LIBXMLPP_LIBS@ @JACK_LIBS@
