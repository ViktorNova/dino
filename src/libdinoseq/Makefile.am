AM_CPPFLAGS = @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @LADCCA_CFLAGS@ @DEBUG_CFLAGS@

noinst_LTLIBRARIES = libdinoseq.la

libdinoseq_la_SOURCES = \
	deleter.cpp deleter.hpp \
	pattern.cpp pattern.hpp \
	track.cpp track.hpp \
	song.cpp song.hpp \
	ringbuffer.hpp \
	sequencer.cpp sequencer.hpp \
	tempomap.cpp tempomap.hpp

libdinoseq_la_LIBADD = @LIBXMLPP_LIBS@ @JACK_LIBS@ @LADCCA_LIBS@
