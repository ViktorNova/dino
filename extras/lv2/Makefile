PACKAGE_NAME = dino-lv2host
PACKAGE_VERSION = 0.1.0
PKG_DEPS = dino>=0.3.10 libslv2>=0.0.1 jack>=0.102.6 liblo>=0.22

#PROGRAMS = dino-lv2host
dino-lv2host_SOURCES = lv2host.hpp lv2host.cpp main.cpp
dino-lv2host_CFLAGS = `pkg-config --cflags libslv2 jack liblo`
dino-lv2host_LDFLAGS = `pkg-config --libs libslv2 jack liblo`

MODULES = lv2hostplugin.so
lv2hostplugin_so_SOURCES = lv2hostplugin.cpp lv2widget.hpp lv2widget.cpp
lv2hostplugin_so_CFLAGS = `pkg-config --cflags libslv2 liblo dino`
lv2hostplugin_so_LDFLAGS = `pkg-config --libs libslv2 liblo dino`
lv2hostplugin_so_INSTALLDIR = `pkg-config --variable=plugindir dino`


include ../../Makefile.template
