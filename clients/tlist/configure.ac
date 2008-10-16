#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(tlist, 0.01, [gam3@gam3.com])
AC_CONFIG_SRCDIR([mainwindow.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lQtCore':
AC_CHECK_LIB([QtCore], [main])
# FIXME: Replace `main' with a function in `-lQtGui':
AC_CHECK_LIB([QtGui], [main])
# FIXME: Replace `main' with a function in `-lQtNetwork':
AC_CHECK_LIB([QtNetwork], [main])
# FIXME: Replace `main' with a function in `-lQtWebKit':
AC_CHECK_LIB([QtWebKit], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.
AC_PATH_X

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_HEADER_STDC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
