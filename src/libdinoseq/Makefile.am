AM_CPPFLAGS = @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @DEBUG_CFLAGS@

noinst_LTLIBRARIES = libdinoseq.la

libdinoseq_la_SOURCES = \
	basemidievent.hpp basemidievent.cpp \
	deletable.hpp \
	deleter.cpp deleter.hpp \
	noteevent.cpp noteevent.hpp \
	pattern.cpp pattern.hpp \
	track.cpp track.hpp \
	song.cpp song.hpp \
	ringbuffer.hpp \
	sequencer.cpp sequencer.hpp \
	tempomap.cpp tempomap.hpp \
	cdtree.hpp \
	debug.hpp debug.cpp

libdinoseq_la_LIBADD = @LIBXMLPP_LIBS@ @JACK_LIBS@
