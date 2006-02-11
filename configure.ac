#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

# Don't touch this line - it's updated automagically by dcvs
AC_INIT(dino, 0.1.170)
AC_CONFIG_HEADERS([src/config.hpp])
AC_DEFINE([CR_YEAR], ["2006"], [The copyright year])
AC_DEFINE([DATA_DIR], ["$pkgdatadir"], [The directory for data files])

AC_PROG_LIBTOOL
AC_CONFIG_SRCDIR([src/gui/main.cpp])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
PKG_CHECK_MODULES(LIBGLADEMM, libglademm-2.4 >= 2.4.1)
PKG_CHECK_MODULES(LIBXMLPP, libxml++-2.6 >= 2.6.1)
PKG_CHECK_MODULES(JACK, jack >= 0.99.0)
PKG_CHECK_MODULES(LASH, lash-1.0 >= 0.5.0)

# Enable debugging?
AC_MSG_CHECKING([whether debugging info should be generated])
AC_ARG_ENABLE(debug,
	[AS_HELP_STRING(--enable-debug, [Enable debugging [default=no]])],
	[debug="$enableval"], [debug=no])
if test "$debug" = "yes"; then
	DEBUG_CFLAGS='-g -fno-default-inline -ansi -pedantic -Wall -Werror'
else
	DEBUG_CFLAGS='-DNDEBUG'
	debug=no
fi
AC_SUBST(DEBUG_CFLAGS)
AC_MSG_RESULT($debug)

# Output
AC_CONFIG_FILES([
	Makefile 
	src/Makefile
	src/libdinoseq/Makefile
	src/gui/Makefile
	pixmaps/Makefile])

AC_OUTPUT
