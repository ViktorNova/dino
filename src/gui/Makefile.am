plugindir = $(pkglibdir)/plugins

# The main program
bin_PROGRAMS = dino
dino_SOURCES = \
	main.cpp \
	dinogui.cpp dinogui.hpp \
	plugininterface.hpp \
	plugininterfaceimplementation.cpp plugininterfaceimplementation.hpp \
	pluginlibrary.cpp pluginlibrary.hpp
dino_LDADD = @LIBGLADEMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la libdinoseq_gui.la
dino_CXXFLAGS = @LIBGLADEMM_CFLAGS@ @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @LASH_CFLAGS@ @DEBUG_CFLAGS@ -I../libdinoseq -DDATA_DIR=\"$(pkgdatadir)\" -DPLUGIN_DIR=\"$(plugindir)\"

# A GUI support library for core plugins
pkglib_LTLIBRARIES = libdinoseq_gui.la 
libdinoseq_gui_la_SOURCES = \
	evilscrolledwindow.hpp \
	ruler.cpp ruler.hpp \
	singletextcombo.cpp singletextcombo.hpp
libdinoseq_gui_la_LIBADD = @GTKMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la
libdinoseq_gui_la_CXXFLAGS = @GTKMM_CFLAGS@ @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @LASH_CFLAGS@ -I../libdinoseq

# The plugins
plugin_LTLIBRARIES = sequenceeditor.la patterneditor.la infoeditor.la 

# The sequence editor
sequenceeditor_la_LDFLAGS = -module -avoid-version
sequenceeditor_la_SOURCES = \
	sequenceeditor.cpp sequenceeditor.hpp \
	tempolabel.cpp tempolabel.hpp \
	tempowidget.cpp tempowidget.hpp \
	trackdialog.cpp trackdialog.hpp \
	tracklabel.hpp tracklabel.cpp \
	trackwidget.cpp trackwidget.hpp
sequenceeditor_la_LIBADD = @GTKMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la libdinoseq_gui.la
sequenceeditor_la_CXXFLAGS = @GTKMM_CFLAGS@ @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @LASH_CFLAGS@ -I../libdinoseq

# The pattern editor
patterneditor_la_LDFLAGS = -module -avoid-version
patterneditor_la_SOURCES = \
	cceditor.cpp cceditor.hpp \
	controllerdialog.cpp controllerdialog.hpp \
	noteeditor.cpp noteeditor.hpp \
	octavelabel.cpp octavelabel.hpp \
	patterndialog.cpp patterndialog.hpp \
	patterneditor.cpp patterneditor.hpp
patterneditor_la_LIBADD = @GTKMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la libdinoseq_gui.la
patterneditor_la_CXXFLAGS = @GTKMM_CFLAGS@ @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @LASH_CFLAGS@ -I../libdinoseq

# The info editor
infoeditor_la_LDFLAGS = -module -avoid-version
infoeditor_la_SOURCES = infoeditor.cpp infoeditor.hpp
infoeditor_la_LIBADD = @GTKMM_LIBS@ @LIBXMLPP_LIBS@ @JACK_LIBS@ @LASH_LIBS@ ../libdinoseq/libdinoseq.la
infoeditor_la_CXXFLAGS = @GTKMM_CFLAGS@ @LIBXMLPP_CFLAGS@ @JACK_CFLAGS@ @LASH_CFLAGS@ -I../libdinoseq
