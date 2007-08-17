PACKAGE_NAME = dino
PACKAGE_VERSION = 0.3.166
PKG_DEPS = \
	gtkmm-2.4>=2.6.4 \
	libxml++-2.6>=2.6.1 \
	jack>=0.102.6 \
	lash-1.0>=0.5.0 \
	dbus-1>=1.1.1 \
	vte>=0.16.6


# Data files
DATA = src/gui/pixmaps/midisaurus.png src/gui/pixmaps/head.png src/gui/pixmaps/tinykeyboard.png
DOCS = AUTHORS COPYING README TODO ChangeLog


# The main program (we need to link it with -Wl,-E to allow RTTI with plugins)
PROGRAMS = dino
dino_SOURCES = \
	action.hpp \
	main.cpp \
	dinodbusobject.cpp dinodbusobject.hpp \
	dinogui.cpp dinogui.hpp \
	plugindialog.cpp plugindialog.hpp \
	plugininterfaceimplementation.cpp plugininterfaceimplementation.hpp \
	plugininterface.hpp \
	pluginlibrary.cpp pluginlibrary.hpp \
	dbus/argument.cpp dbus/argument.hpp \
	dbus/connection.cpp dbus/connection.hpp \
	dbus/object.cpp dbus/object.hpp
dino_HEADERS = plugininterface.hpp action.hpp
dino_SOURCEDIR = src/gui
dino_CFLAGS = `pkg-config --cflags gtkmm-2.4 jack libxml++-2.6 lash-1.0 dbus-1` -Isrc/libdinoseq -Isrc
dino_LDFLAGS = `pkg-config --libs gtkmm-2.4 lash-1.0 dbus-1` -Wl,-E
dino_LIBRARIES = src/libdinoseq/libdinoseq.so
main_cpp_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)\" -DVERSION=\"$(PACKAGE_VERSION)\" -DCR_YEAR=\"2005-2007\"
dinogui_cpp_CFLAGS = $(main_cpp_CFLAGS)
pluginlibrary_cpp_CFLAGS = -DPLUGIN_DIR=\"$(pkglibdir)\"


# Shared libraries
LIBRARIES = libdinoseq.so libdinoseq_gui.so

# The library with the sequencer and the song structures
libdinoseq_so_SOURCES = \
	cdtree.hpp \
	command.cpp command.hpp \
	commandproxy.cpp commandproxy.hpp \
	compoundcommand.cpp compoundcommand.hpp \
	controllerinfo.cpp controllerinfo.hpp \
	controller_numbers.hpp \
	curve.cpp curve.hpp \
	debug.cpp debug.hpp \
	deleter.hpp \
	genericcommands.hpp \
	instrumentinfo.cpp instrumentinfo.hpp \
	interpolatedevent.cpp interpolatedevent.hpp \
	midibuffer.cpp midibuffer.hpp \
	note.cpp note.hpp \
	notecollection.cpp notecollection.hpp \
	noteevent.cpp noteevent.hpp \
	pattern.cpp pattern.hpp \
	patternselection.cpp patternselection.hpp \
	recorder.cpp recorder.hpp \
	ringbuffer.hpp \
	sequencable.hpp \
	sequencer.cpp sequencer.hpp \
	song.cpp song.hpp \
	songcommands.cpp songcommands.hpp \
	tempomap.cpp tempomap.hpp \
	track.cpp track.hpp \
	xmlserialisable.hpp
libdinoseq_so_HEADERS = \
	cdtree.hpp \
	command.hpp \
	commandproxy.hpp \
	compoundcommand.hpp \
	controllerinfo.hpp \
	controller_numbers.hpp \
	curve.hpp \
	debug.hpp \
	deleter.hpp \
	genericcommands.hpp \
	instrumentinfo.hpp \
	interpolatedevent.hpp \
	midibuffer.hpp \
	note.hpp \
	notecollection.hpp \
	noteevent.hpp \
	pattern.hpp \
	patternselection.hpp \
	recorder.hpp \
	ringbuffer.hpp \
	sequencable.hpp \
	sequencer.hpp \
	song.hpp \
	songcommands.hpp \
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
	curveeditor.cpp curveeditor.hpp \
	evilscrolledwindow.hpp \
	ruler.cpp ruler.hpp \
	singletextcombo.cpp singletextcombo.hpp
libdinoseq_gui_so_SOURCEDIR = src/gui/libdinoseq_gui
libdinoseq_gui_so_LDFLAGS = `pkg-config --libs gtkmm-2.4`
libdinoseq_gui_so_LIBRARIES = src/libdinoseq/libdinoseq.so
libdinoseq_gui_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6` -Isrc/libdinoseq


# Script interpreters and related files
DATAPACKS = scriptinterpreters
scriptinterpreters_FILES = Python.interpreter dino_dbus_wrapper.py
scriptinterpreters_SOURCEDIR = scripts/interpreters
scriptinterpreters_INSTALLDIR = $(pkgdatadir)/interpreters


# The GUI plugins
MODULES = arrangementeditor.so patterneditor.so infoeditor.so coreactions.so scriptinterface.so

# The sequence editor
arrangementeditor_so_SOURCES = \
	controllerdialog.cpp controllerdialog.hpp \
	arrangementeditor.cpp arrangementeditor.hpp \
	tempolabel.cpp tempolabel.hpp \
	tempowidget.cpp tempowidget.hpp \
	trackdialog.cpp trackdialog.hpp \
	tracklabel.cpp tracklabel.hpp \
	trackwidget.cpp trackwidget.hpp \
	sequencewidget.cpp sequencewidget.hpp
arrangementeditor_so_SOURCEDIR = src/gui/arrangementeditor
arrangementeditor_so_LDFLAGS = `pkg-config --libs gtkmm-2.4`
arrangementeditor_so_LIBRARIES = src/gui/libdinoseq_gui/libdinoseq_gui.so src/libdinoseq/libdinoseq.so
arrangementeditor_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6 jack lash-1.0` -Isrc/libdinoseq -Isrc/gui -Isrc/gui/libdinoseq_gui
tracklabel_cpp_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)\"

# The pattern editor
patterneditor_so_SOURCES = \
	noteeditor.cpp noteeditor.hpp \
	octavelabel.cpp octavelabel.hpp \
	patterndialog.cpp patterndialog.hpp \
	patterneditor.cpp patterneditor.hpp
patterneditor_so_SOURCEDIR = src/gui/patterneditor
patterneditor_so_LDFLAGS = `pkg-config --libs gtkmm-2.4`
patterneditor_so_LIBRARIES = src/gui/libdinoseq_gui/libdinoseq_gui.so src/libdinoseq/libdinoseq.so
patterneditor_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6 jack lash-1.0` -Isrc/libdinoseq -Isrc/gui -Isrc/gui/libdinoseq_gui

# The info editor
infoeditor_so_SOURCES = infoeditor.cpp infoeditor.hpp
infoeditor_so_SOURCEDIR = src/gui/infoeditor
infoeditor_so_LDFLAGS = `pkg-config --libs gtkmm-2.4`
infoeditor_so_LIBRARIES = src/gui/libdinoseq_gui/libdinoseq_gui.so src/libdinoseq/libdinoseq.so
infoeditor_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6 jack lash-1.0` -Isrc/libdinoseq -Isrc/gui -Isrc/gui/libdinoseq_gui

# Core actions
coreactions_so_SOURCES = coreactions.cpp
coreactions_so_SOURCEDIR = src/gui/coreactions
coreactions_so_LDFLAGS = `pkg-config --libs gtkmm-2.4`
coreactions_so_LIBRARIES = src/gui/libdinoseq_gui/libdinoseq_gui.so src/libdinoseq/libdinoseq.so
coreactions_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6 jack lash-1.0` -Isrc/libdinoseq -Isrc/gui

# Script interface
scriptinterface_so_SOURCES = \
	scriptwidget.hpp scriptwidget.cpp
scriptinterface_so_SOURCEDIR = src/gui/scriptinterface
scriptinterface_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 vte`
scriptinterface_so_LIBRARIES = src/libdinoseq/libdinoseq.so
scriptinterface_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6 jack lash-1.0 vte` -Isrc/libdinoseq -Isrc/gui -DINTERPRETERDIR=\"$(pkgdatadir)/interpreters\"


# Do the magic
include Makefile.template
