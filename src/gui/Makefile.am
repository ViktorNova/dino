AM_CPPFLAGS = @LIBGLADEMM_CFLAGS@ @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @LASH_CFLAGS@ @DEBUG_CFLAGS@ -I"$(top_srcdir)/src/libdinoseq" -DDATA_DIR=\"$(pkgdatadir)\"

bin_PROGRAMS = dino

dino_SOURCES = \
	main.cpp \
	cceditor.cpp cceditor.hpp \
	controllerdialog.cpp controllerdialog.hpp \
	dinogui.cpp dinogui.hpp \
	evilscrolledwindow.hpp \
	infoeditor.cpp infoeditor.hpp \
	noteeditor.cpp noteeditor.hpp \
	octavelabel.cpp octavelabel.hpp \
	patterneditor.cpp patterneditor.hpp \
	patterndialog.cpp patterndialog.hpp \
	ruler.cpp ruler.hpp \
	sequenceeditor.cpp sequenceeditor.hpp \
	singletextcombo.cpp singletextcombo.hpp \
	tempolabel.cpp tempolabel.hpp \
	tempowidget.cpp tempowidget.hpp \
	trackdialog.cpp trackdialog.hpp \
	tracklabel.hpp tracklabel.cpp \
	trackwidget.cpp trackwidget.hpp

dino_LDADD = @LIBGLADEMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la
