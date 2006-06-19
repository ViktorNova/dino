PACKAGE_NAME = dino-lv2host
PACKAGE_VERSION = 0.1.0
PKG_DEPS = libslv2>=0.0.1 jack>=0.102.6 liblo>=0.22

PROGRAMS = dino-lv2host
dino-lv2host_SOURCES = lv2host.hpp lv2host.cpp main.cpp
dino-lv2host_CFLAGS = `pkg-config --cflags libslv2 jack liblo`
dino-lv2host_LDFLAGS = `pkg-config --libs libslv2 jack liblo`


# Do the magic
include ../../Makefile.template
