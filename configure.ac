#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(dino, 0.0)
AC_CONFIG_SRCDIR([src/main.cpp])
# AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
PKG_CHECK_MODULES(LIBGLADEMM, libglademm-2.4 >= 2.4.1)
PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= 2.4.6)
PKG_CHECK_MODULES(LIBXMLPP, libxml++-2.6 >= 2.6.1)
PKG_CHECK_MODULES(ALSA, alsa >= 1.0.6)
PKG_CHECK_MODULES(JACK, jack >= 0.99.0)
PKG_CHECK_MODULES(LADCCA, ladcca-1.0 >= 0.4.0)

DEBUG_CFLAGS=-fno-default-inline
AC_SUBST(DEBUG_CFLAGS)
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Output
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
