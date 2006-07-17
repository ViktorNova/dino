PACKAGE_NAME = lv2host
PACKAGE_VERSION = 0.1.3
PKG_DEPS = libslv2>=0.0.1 jack>=0.102.6 liblo>=0.22

PROGRAMS = lv2host
lv2host_SOURCES = lv2host.hpp lv2host.cpp main.cpp lv2-miditype.h
lv2host_CFLAGS = `pkg-config --cflags libslv2 jack liblo`
lv2host_LDFLAGS = `pkg-config --libs libslv2 jack liblo`


include ../../Makefile.template
