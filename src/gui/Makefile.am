AM_CPPFLAGS = @LIBGLADEMM_CFLAGS@ @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @LASH_CFLAGS@ @DEBUG_CFLAGS@ -I../libdinoseq -DDATA_DIR=\"$(pkgdatadir)\"

bin_PROGRAMS = dino

dino_SOURCES = \
	main.cpp \
	cceditor.cpp cceditor.hpp \
	controllerdialog.cpp controllerdialog.hpp \
	dinogui.cpp dinogui.hpp \
	evilscrolledwindow.hpp \
	noteeditor.cpp noteeditor.hpp \
	octavelabel.cpp octavelabel.hpp \
	patterneditor.cpp patterneditor.hpp \
	patterndialog.cpp patterndialog.hpp \
	plugininterface.hpp \
	plugininterfaceimplementation.cpp plugininterfaceimplementation.hpp \
	pluginlibrary.cpp pluginlibrary.hpp \
	ruler.cpp ruler.hpp \
	sequenceeditor.cpp sequenceeditor.hpp \
	singletextcombo.cpp singletextcombo.hpp \
	tempolabel.cpp tempolabel.hpp \
	tempowidget.cpp tempowidget.hpp \
	trackdialog.cpp trackdialog.hpp \
	tracklabel.hpp tracklabel.cpp \
	trackwidget.cpp trackwidget.hpp

dino_LDADD = @LIBGLADEMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la

lib_LTLIBRARIES = infoeditor.la

infoeditor_la_LDFLAGS = -module -avoid-version
infoeditor_la_SOURCES = infoeditor.cpp infoeditor.hpp
infoeditor_la_LIBADD = @LIBGLADEMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la
