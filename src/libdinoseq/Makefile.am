AM_CPPFLAGS = @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @DEBUG_CFLAGS@

noinst_LTLIBRARIES = libdinoseq.la

libdinoseq_la_SOURCES = \
	ccevent.cpp ccevent.hpp \
	controller.cpp controller.hpp \
	deletable.cpp deletable.hpp \
	deleter.hpp \
	midievent.hpp midievent.cpp \
	note.cpp note.hpp \
	noteevent.cpp noteevent.hpp \
	pattern.cpp pattern.hpp \
	track.cpp track.hpp \
	song.cpp song.hpp \
	ringbuffer.hpp \
	sequencer.cpp sequencer.hpp \
	tempomap.cpp tempomap.hpp \
	cdtree.hpp \
	debug.hpp debug.cpp \
	xmlserialisable.hpp

libdinoseq_la_LIBADD = @LIBXMLPP_LIBS@ @JACK_LIBS@
