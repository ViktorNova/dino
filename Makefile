CXXFLAGS = -std=c++0x

PACKAGE_NAME = dino
PACKAGE_VERSION = $(shell ./VERSION)
PKG_DEPS = \
	glib-2.0>=2.22.4


# Data files
#DATA = src/gui/pixmaps/midisaurus.png src/gui/pixmaps/head.png src/gui/pixmaps/tinykeyboard.png
DOCS = AUTHORS COPYING HACKING README TODO ChangeLog

TESTS = src/test/libdinoseq/libdinoseq_test
TESTFLAGS = -r detailed -l all

# The main program (we need to link it with -Wl,-E to allow RTTI with plugins)
PROGRAMS = libdinoseq_test #dino
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
	dbus/object.cpp dbus/object.hpp \
	dbus/proxy.cpp dbus/proxy.hpp
dino_HEADERS = plugininterface.hpp action.hpp
dino_SOURCEDIR = src/gui
dino_CFLAGS = `pkg-config --cflags gtkmm-2.4 jack libxml++-2.6 lash-1.0 dbus-1` -Isrc/libdinoseq -Isrc
dino_LDFLAGS = `pkg-config --libs gtkmm-2.4 lash-1.0 dbus-1` -Wl,-E
dino_LIBRARIES = src/libdinoseq/libdinoseq.so
main_cpp_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)\" -DVERSION=\"$(PACKAGE_VERSION)\" -DCR_YEAR=\"2005-2009\"
dinogui_cpp_CFLAGS = $(main_cpp_CFLAGS)
pluginlibrary_cpp_CFLAGS = -DPLUGIN_DIR=\"$(pkglibdir)\"


# Shared libraries
LIBRARIES = libdinoseq.so #libdinoseq_gui.so


# The library with the sequencer and the song structures
libdinoseq_so_SOURCES = \
	atomicint.cpp atomicint.hpp \
	curve.cpp curve.hpp \
	ostreambuffer.cpp ostreambuffer.hpp \
	sequencable.cpp sequencable.hpp \
	sequencer.cpp sequencer.hpp \
	songtime.cpp songtime.hpp
libdinoseq_so_HEADERS = \
	atomicptr.hpp \
	eventbuffer.hpp \
	linkedlist.hpp \
	meta.hpp \
	nodelist.hpp \
	nodequeue.hpp \
	nodeskiplist.hpp \
	tempomap.hpp
libdinoseq_so_SOURCEDIR = src/libdinoseq
libdinoseq_so_CFLAGS = `pkg-config --cflags glib-2.0`
libdinoseq_so_LDFLAGS = `pkg-config --libs glib-2.0`

# pkg-config file for libdinoseq.so
#PCFILES = dino.pc

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
#DATAPACKS = scriptinterpreters
scriptinterpreters_FILES = Python.interpreter dino_dbus_wrapper.py
scriptinterpreters_SOURCEDIR = scripts/interpreters
scriptinterpreters_INSTALLDIR = $(pkgdatadir)/interpreters


# The GUI plugins
#MODULES = arrangementeditor.so patterneditor.so infoeditor.so coreactions.so scriptinterface.so debugging.so

# The sequence editor
arrangementeditor_so_SOURCES = \
	arrangementeditor.cpp arrangementeditor.hpp \
	controllerdialog.cpp controllerdialog.hpp \
	keydialog.cpp keydialog.hpp \
	tempolabel.cpp tempolabel.hpp \
	sequencewidget.cpp sequencewidget.hpp \
	tempowidget.cpp tempowidget.hpp \
	trackdialog.cpp trackdialog.hpp \
	tracklabel.cpp tracklabel.hpp \
	trackwidget.cpp trackwidget.hpp
arrangementeditor_so_SOURCEDIR = src/gui/arrangementeditor
arrangementeditor_so_LDFLAGS = `pkg-config --libs gtkmm-2.4`
arrangementeditor_so_LIBRARIES = src/gui/libdinoseq_gui/libdinoseq_gui.so src/libdinoseq/libdinoseq.so
arrangementeditor_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6 jack lash-1.0` -Isrc/libdinoseq -Isrc/gui -Isrc/gui/libdinoseq_gui
tracklabel_cpp_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)\"

# The pattern editor
patterneditor_so_SOURCES = \
	curveeditor2.cpp curveeditor2.hpp \
	noteeditor.cpp noteeditor.hpp \
	noteeditor2.cpp noteeditor2.hpp \
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

# Debugging
debugging_so_SOURCES = \
	debugging.cpp \
	debuggingpage.cpp debuggingpage.hpp
debugging_so_SOURCEDIR = src/gui/debugging
debugging_so_LDFLAGS = `pkg-config --libs gtkmm-2.4`
debugging_so_LIBRARIES = src/gui/libdinoseq_gui/libdinoseq_gui.so src/libdinoseq/libdinoseq.so
debugging_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6 jack lash-1.0` -Isrc/libdinoseq -Isrc/gui -Isrc/gui/libdinoseq_gui


# Test modules

libdinoseq_test_SOURCES = \
	libdinoseq_test.cpp \
	atomicint_test.cpp \
	atomicptr_test.cpp \
	curve_test.cpp \
	linkedlist_test.cpp \
	meta_test.cpp \
	nodelist_test.cpp \
	nodequeue_test.cpp \
	nodeskiplist_test.cpp \
	ostreambuffer_test.cpp \
	sequencer_test.cpp \
	songtime_test.cpp
libdinoseq_test_SOURCEDIR = src/test/libdinoseq
libdinoseq_test_CFLAGS = -Isrc/libdinoseq `pkg-config --cflags glib-2.0` -DBOOST_TEST_DYN_LINK
libdinoseq_test_LDFLAGS = -Wl,-E `pkg-config --libs glib-2.0` -lboost_unit_test_framework
libdinoseq_test_LIBRARIES = $(BUILDPREFIX)src/libdinoseq/libdinoseq.so
libdinoseq_test_NOINST = true


# Do the magic
include Makefile.template

dox:
	cat Doxyfile | sed s@VERSION_SUBST@$(PACKAGE_VERSION)@ > Doxyfile.subst
	doxygen Doxyfile.subst
	rm Doxyfile.subst

