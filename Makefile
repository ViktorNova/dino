PACKAGE_NAME = dino
PACKAGE_VERSION = 0.3.66
PKG_DEPS = \
	gtkmm-2.4>=2.6.4 \
	libxml++-2.6>=2.6.1 \
	jack>=0.102.6 \
	lash-1.0>=0.5.0


# Data files
DATA = src/gui/pixmaps/midisaurus.png src/gui/pixmaps/head.png src/gui/pixmaps/tinykeyboard.png
DOCS = AUTHORS COPYING README TODO ChangeLog


# The main program (we need to link it with -Wl,-E to allow RTTI with plugins)
PROGRAMS = dino
dino_SOURCES = \
	action.hpp \
	evilscrolledwindow.hpp \
	main.cpp \
	dinogui.cpp dinogui.hpp \
	plugindialog.cpp plugindialog.hpp \
	plugininterfaceimplementation.cpp plugininterfaceimplementation.hpp \
	plugininterface.hpp \
	pluginlibrary.cpp pluginlibrary.hpp 
dino_HEADERS = plugininterface.hpp action.hpp
dino_SOURCEDIR = src/gui
dino_CFLAGS = `pkg-config --cflags gtkmm-2.4 jack libxml++-2.6 lash-1.0` -Isrc/libdinoseq -Isrc
dino_LDFLAGS = `pkg-config --libs gtkmm-2.4 lash-1.0` -Wl,-E -Lsrc/gui -Lsrc/libdinoseq -ldinoseq
main_cpp_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)\" -DVERSION=\"$(PACKAGE_VERSION)\" -DCR_YEAR=\"2006\"
dinogui_cpp_CFLAGS = $(main_cpp_CFLAGS)
pluginlibrary_cpp_CFLAGS = -DPLUGIN_DIR=\"$(pkglibdir)\"

# Shared libraries
LIBRARIES = libdinoseq.so libdinoseq_gui.so

# The library with the sequencer and the song structures
libdinoseq_so_SOURCES = \
	cdtree.hpp \
	controller_numbers.hpp \
	controller.cpp controller.hpp \
	debug.cpp debug.hpp \
	deleter.hpp \
	interpolatedevent.cpp interpolatedevent.hpp \
	midibuffer.cpp midibuffer.hpp \
	note.cpp note.hpp \
	notecollection.cpp notecollection.hpp \
	noteevent.cpp noteevent.hpp \
	pattern.cpp pattern.hpp \
	patternselection.cpp patternselection.hpp \
	recorder.cpp \
	ringbuffer.hpp \
	sequencer.cpp sequencer.hpp \
	song.cpp song.hpp \
	tempomap.cpp tempomap.hpp \
	track.cpp track.hpp \
	xmlserialisable.hpp
libdinoseq_so_HEADERS = \
	cdtree.hpp \
	controller_numbers.hpp \
	controller.hpp \
	debug.hpp \
	deleter.hpp \
	interpolatedevent.hpp \
	midibuffer.hpp \
	note.hpp \
	notecollection.hpp \
	noteevent.hpp \
	pattern.hpp \
	patternselection.hpp \
	recorder.hpp \
	ringbuffer.hpp \
	sequencer.hpp \
	song.hpp \
	tempomap.hpp \
	track.hpp \
	xmlserialisable.hpp
libdinoseq_so_SOURCEDIR = src/libdinoseq
libdinoseq_so_CFLAGS = `pkg-config --cflags libxml++-2.6 jack`
libdinoseq_so_LDFLAGS = `pkg-config --libs libxml++-2.6 jack`

# pkg-config file for libdinoseq.so
PCFILES = dino.pc

# A GUI support library for core plugins
libdinoseq_gui_so_SOURCES = \
	ruler.cpp ruler.hpp \
	singletextcombo.cpp singletextcombo.hpp
libdinoseq_gui_so_SOURCEDIR = src/gui
libdinoseq_gui_so_LDFLAGS = `pkg-config --libs gtkmm-2.4` -Lsrc/libdinoseq -ldinoseq
libdinoseq_gui_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6` -Isrc/libdinoseq


# The GUI plugins
MODULES = sequenceeditor.so patterneditor.so infoeditor.so coreactions.so

# The sequence editor
sequenceeditor_so_SOURCES = \
	sequenceeditor.cpp sequenceeditor.hpp \
	tempolabel.cpp tempolabel.hpp \
	tempowidget.cpp tempowidget.hpp \
	trackdialog.cpp trackdialog.hpp \
	tracklabel.cpp tracklabel.hpp \
	trackwidget.cpp trackwidget.hpp
sequenceeditor_so_SOURCEDIR = src/gui
sequenceeditor_so_LDFLAGS = `pkg-config --libs gtkmm-2.4` -Lsrc/gui -Lsrc/libdinoseq -ldinoseq_gui -ldinoseq
sequenceeditor_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6 jack lash-1.0` -Isrc/libdinoseq
tracklabel_cpp_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)\"

# The pattern editor
patterneditor_so_SOURCES = \
	cceditor.cpp cceditor.hpp \
	controllerdialog.cpp controllerdialog.hpp \
	noteeditor.cpp noteeditor.hpp \
	octavelabel.cpp octavelabel.hpp \
	patterndialog.cpp patterndialog.hpp \
	patterneditor.cpp patterneditor.hpp
patterneditor_so_SOURCEDIR = src/gui
patterneditor_so_LDFLAGS = `pkg-config --libs gtkmm-2.4` -Lsrc/gui -Lsrc/libdinoseq -ldinoseq_gui -ldinoseq
patterneditor_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6 jack lash-1.0` -Isrc/libdinoseq

# The info editor
infoeditor_so_SOURCES = infoeditor.cpp infoeditor.hpp
infoeditor_so_SOURCEDIR = src/gui
infoeditor_so_LDFLAGS = `pkg-config --libs gtkmm-2.4` -Lsrc/gui -Lsrc/libdinoseq -ldinoseq_gui -ldinoseq
infoeditor_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6 jack lash-1.0` -Isrc/libdinoseq

# Core actions
coreactions_so_SOURCES = coreactions.cpp
coreactions_so_SOURCEDIR = src/gui
coreactions_so_LDFLAGS = `pkg-config --libs gtkmm-2.4` -Lsrc/gui -Lsrc/libdinoseq -ldinoseq
coreactions_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6 jack lash-1.0` -Isrc/libdinoseq



# Do the magic
include Makefile.template
