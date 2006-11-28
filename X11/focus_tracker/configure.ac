#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT(focus-tracker, [0.0.1], [tasker-bugs@gam3.net])
AC_CONFIG_SRCDIR([focus.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lX11':
AC_CHECK_LIB([X11], [main])
# FIXME: Replace `main' with a function in `-lXmu':
AC_CHECK_LIB([Xmu], [main])
# FIXME: Replace `main' with a function in `-lXss':
AC_CHECK_LIB([Xss], [main])
# FIXME: Replace `main' with a function in `-lXt':
AC_CHECK_LIB([Xt], [main])

# Checks for header files.
AC_PATH_X
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h locale.h netinet/in.h stdlib.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([bzero gethostname memmove memset setlocale socket strchr strdup])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
