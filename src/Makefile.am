AM_CPPFLAGS = @LIBGLADEMM_CFLAGS@ @ALSA_CFLAGS@ @JACK_CFLAGS@

bin_PROGRAMS = dino

dino_SOURCES = \
	main.cpp \
	dino.cpp dino.hpp \
	pattern.cpp pattern.hpp \
	track.cpp track.hpp \
	song.cpp song.hpp \
	patterneditor.cpp patterneditor.hpp \
	cceditor.cpp cceditor.hpp \
	sequencer.cpp sequencer.hpp \
	trackwidget.cpp trackwidget.hpp \
	evilscrolledwindow.hpp

dino_LDADD = @LIBGLADEMM_LIBS@ @ALSA_LIBS@ @JACK_LIBS@