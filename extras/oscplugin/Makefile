PACKAGE_NAME = dino-oscplugin
PACKAGE_VERSION = 0.3.10
PKG_DEPS = dino>=0.3.10 liblo>=0.22

MODULES = oscplugin.so
oscplugin_so_SOURCES = oscplugin.cpp
oscplugin_so_CFLAGS = `pkg-config --cflags dino liblo`
oscplugin_so_LDFLAGS = `pkg-config --libs dino liblo`
oscplugin_so_INSTALLDIR = `pkg-config --variable=plugindir dino`

include ../../Makefile.template