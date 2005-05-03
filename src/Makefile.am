AM_CPPFLAGS = @LIBGLADEMM_CFLAGS@ @GTHREAD_CFLAGS@ @LIBXMLPP_CFLAGS@ @ALSA_CFLAGS@ @JACK_CFLAGS@ @LADCCA_CFLAGS@ @DEBUG_CFLAGS@

bin_PROGRAMS = dino

dino_SOURCES = \
	main.cpp \
	dino.cpp dino.hpp \
	jackclient.hpp jackclient.cpp \
	pattern.cpp pattern.hpp \
	track.cpp track.hpp \
	song.cpp song.hpp \
	octavelabel.cpp octavelabel.hpp \
	patterneditor.cpp patterneditor.hpp \
	cceditor.cpp cceditor.hpp \
	ruler.cpp ruler.hpp \
	sequencer.cpp sequencer.hpp \
	singletextcombo.cpp singletextcombo.hpp \
	tempolabel.cpp tempolabel.hpp \
	tempowidget.cpp tempowidget.hpp \
	tracklabel.hpp tracklabel.cpp \
	trackwidget.cpp trackwidget.hpp \
	evilscrolledwindow.hpp

dino_LDADD = @LIBGLADEMM_LIBS@ @GTHREAD_LIBS@ @LIBXMLPP_LIBS@ @ALSA_LIBS@ @JACK_LIBS@ @LADCCA_LIBS@
