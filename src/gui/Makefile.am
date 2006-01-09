AM_CPPFLAGS = @LIBGLADEMM_CFLAGS@ @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @LASH_CFLAGS@ @DEBUG_CFLAGS@ -I../libdinoseq

bin_PROGRAMS = dino

dino_SOURCES = \
	main.cpp \
	dinogui.cpp dinogui.hpp \
	evilscrolledwindow.hpp \
	octavelabel.cpp octavelabel.hpp \
	patterneditor.cpp patterneditor.hpp \
	ruler.cpp ruler.hpp \
	singletextcombo.cpp singletextcombo.hpp \
	tempolabel.cpp tempolabel.hpp \
	tempowidget.cpp tempowidget.hpp \
	tracklabel.hpp tracklabel.cpp \
	trackwidget.cpp trackwidget.hpp

dino_LDADD = @LIBGLADEMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la
