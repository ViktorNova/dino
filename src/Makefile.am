AM_CPPFLAGS = @LIBGLADEMM_CFLAGS@ @GTHREAD_CFLAGS@ @ALSA_CFLAGS@ @JACK_CFLAGS@

bin_PROGRAMS = dino

dino_SOURCES = \
	main.cpp \
	dino.cpp dino.hpp \
	pattern.cpp pattern.hpp \
	track.cpp track.hpp \
	song.cpp song.hpp \
	patterneditor.cpp patterneditor.hpp \
	cceditor.cpp cceditor.hpp \
	ruler.cpp ruler.hpp \
	sequencer.cpp sequencer.hpp \
	singletextcombo.cpp singletextcombo.hpp \
	trackwidget.cpp trackwidget.hpp \
	evilscrolledwindow.hpp

dino_LDADD = @LIBGLADEMM_LIBS@ @GTHREAD_LIBS@ @ALSA_LIBS@ @JACK_LIBS@