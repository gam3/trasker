#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT(tlist, 0.0.1, tasker-bugs@gam3.net)
AC_CONFIG_SRCDIR([tlist.cpp])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h stdlib.h sys/socket.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

dnl qt
PKG_CHECK_MODULES(QT, qt-mt >= 3.0, [ QT_VERSION=3 ], [
    PKG_CHECK_MODULES(QT, qt >= 3.0, [QT_VERSION=3])
  ])
AC_CHECK_PROG(MOC, [moc], [moc])
dnl end qt

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
