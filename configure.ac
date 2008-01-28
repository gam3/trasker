#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([tasker],[0.0.1],[tasker-bugs@gam3.net])
AC_PREREQ(2.59)
AC_COPYRIGHT([Copyright (c) 2006 G. Allen Morris III])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

# Checks for programs.
dnl AC_PROG_CC
dnl AC_PROG_CXX

# Checks for libraries.
dnl AC_PATH_X
dnl AC_PATH_XTRA

# Checks for header files.
dnl AC_CHECK_HEADERS(fcntl.h locale.h sys/ioctl.h, , AC_MSG_ERROR(Missing headers))

dnl See if we can find OpenSSL
dnl We can compile without OpenSSL if we have to
dnl ac_use_openssl="true"
dnl ac_openssl_lib_dir="/usr/lib /usr/local /usr/local/ssl /usr/local/ssl/lib /usr/pkg"
dnl ac_openssl_inc_dir="/usr/include /usr/local /usr/local/ssl /usr/pkg /usr/local/ssl/include"

dnl qt
dnl PKG_CHECK_MODULES(QT, qt-mt >= 3.0, [ QT_VERSION=3 ], [
dnl    PKG_CHECK_MODULES(QT, qt >= 3.0, [QT_VERSION=3])
dnl  ])
dnl AC_CHECK_PROG(MOC, [moc], [moc])
dnl end qt

dnl qt
dnl PKG_CHECK_MODULES(QT, qt-mt >= 4.0, [ QT_VERSION=4 ], [
dnl    PKG_CHECK_MODULES(QT, qt >= 4.0, [QT_VERSION=4])
dnl  ])
dnl AC_CHECK_PROG(MOC, [moc], [moc])
dnl end qt

dnl AC_ARG_WITH(openssl,[  --with-openssl            root location for OpenSSL],
dnl     if test "$withval" = "no"; then
dnl 	ac_use_openssl="false"
dnl     else
dnl 	ac_openssl_lib_dir="$withval/lib $withval"
dnl 	ac_openssl_inc_dir=$withval/include
dnl     fi
dnl )

dnl AC_ARG_WITH(openssl-inc,[  --with-openssl-inc        OpenSSL include files],
dnl     ac_openssl_inc_dir=$withval
dnl )
dnl AC_ARG_WITH(openssl-lib,[  --with-openssl-lib        OpenSSL library files],
dnl     ac_openssl_lib_dir=$withval
dnl )

#
# Perl specific
#
AC_ARG_WITH(perl-modules,
[
Perl:
  --with-perl-modules[=ARGS]      Install the perl modules along with the rest
                                  of the tasker toolkit.  If ARGS is
                                  specified, they're passed to the
                                  Makefile.PL script.],[
    install_perl=yes
    if test "$withval" = yes; then
      PERLARGS=""
    else
      PERLARGS="$withval"
    fi
])

# Checks for typedefs, structures, and compiler characteristics.
dnl AC_C_CONST
dnl AC_TYPE_PID_T
dnl AC_TYPE_SIGNAL

# Checks for library functions.
dnl AC_FUNC_FORK
dnl AC_PROG_GCC_TRADITIONAL
dnl AC_FUNC_MALLOC
dnl AC_HEADER_STDC
dnl AC_FUNC_REALLOC
dnl AC_CHECK_FUNCS([memset setlocale])
dnl AC_CHECK_FUNCS([gethostname])
dnl AC_CHECK_FUNCS([strchr])

dnl AM_PATH_GLIB_2_0()

AC_CONFIG_SUBDIRS( X11 gtk qt servers )

AC_CONFIG_FILES([Makefile docs/Makefile perl/Makefile ])

AC_OUTPUT
