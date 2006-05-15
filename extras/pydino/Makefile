PACKAGE_NAME = pydino
PACKAGE_VERSION = 0.3.10
PKG_DEPS = dino>=0.3.10
DEPDIRS = -I/usr/include/python2.4
PYTHON_EXTENSION_DIR = $(shell (echo import sys; echo print sys.exec_prefix + \'/lib/python2.4/site-packages\') | python)

MODULES = dino.so

dino_so_SOURCES = pydino.cpp signalwrappers.hpp
dino_so_CFLAGS = `pkg-config --cflags dino` -I/usr/include/python2.4
dino_so_LDFLAGS = `pkg-config --libs dino` -lboost_python -lpython2.4
dino_so_INSTALLDIR = $(PYTHON_EXTENSION_DIR)

include ../../Makefile.template