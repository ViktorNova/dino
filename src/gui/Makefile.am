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
	pluginlibrary.cpp pluginlibrary.hpp

dino_LDADD = @LIBGLADEMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la libdinoseq_gui.la

lib_LTLIBRARIES = libdinoseq_gui.la sequenceeditor.la infoeditor.la

libdinoseq_gui_la_LIBADD = @LIBGLADEMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la
libdinoseq_gui_la_SOURCES = \
	ruler.cpp ruler.hpp \
	singletextcombo.cpp singletextcombo.hpp

sequenceeditor_la_LDFLAGS = -module -avoid-version
sequenceeditor_la_SOURCES = \
	sequenceeditor.cpp sequenceeditor.hpp \
	tempolabel.cpp tempolabel.hpp \
	tempowidget.cpp tempowidget.hpp \
	trackdialog.cpp trackdialog.hpp \
	tracklabel.hpp tracklabel.cpp \
	trackwidget.cpp trackwidget.hpp
sequenceeditor_la_LIBADD = @LIBGLADEMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la libdinoseq_gui.la

infoeditor_la_LDFLAGS = -module -avoid-version
infoeditor_la_SOURCES = infoeditor.cpp infoeditor.hpp
infoeditor_la_LIBADD = @LIBGLADEMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la
