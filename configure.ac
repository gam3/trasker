#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([tasker],[0.0.1],[tasker-bugs@gam3.net])
AC_PREREQ(2.59)
AC_COPYRIGHT([Copyright (c) 2006 G. Allen Morris III])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.
AC_PATH_X
AC_PATH_XTRA

# Checks for header files.
AC_CHECK_HEADERS(fcntl.h locale.h sys/ioctl.h, , AC_MSG_ERROR(Missing headers))

dnl See if we can find OpenSSL
dnl We can compile without OpenSSL if we have to
ac_use_openssl="true"
ac_openssl_lib_dir="/usr/lib /usr/local /usr/local/ssl /usr/local/ssl/lib /usr/pkg"
ac_openssl_inc_dir="/usr/include /usr/local /usr/local/ssl /usr/pkg /usr/local/ssl/include"

dnl qt
PKG_CHECK_MODULES(QT, qt-mt >= 3.0, [ QT_VERSION=3 ], [
    PKG_CHECK_MODULES(QT, qt >= 3.0, [QT_VERSION=3])
  ])
AC_CHECK_PROG(MOC, [moc], [moc])
dnl end qt

AC_ARG_WITH(openssl,[  --with-openssl            root location for OpenSSL],
    if test "$withval" = "no"; then
	ac_use_openssl="false"
    else
	ac_openssl_lib_dir="$withval/lib $withval"
	ac_openssl_inc_dir=$withval/include
    fi
)

AC_ARG_WITH(openssl-inc,[  --with-openssl-inc        OpenSSL include files],
    ac_openssl_inc_dir=$withval
)
AC_ARG_WITH(openssl-lib,[  --with-openssl-lib        OpenSSL library files],
    ac_openssl_lib_dir=$withval
)

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
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIGNAL

# Checks for library functions.
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_HEADER_STDC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset setlocale])
AC_CHECK_FUNCS([gethostname])
AC_CHECK_FUNCS([strchr])

AM_PATH_GLIB_2_0()

AC_CONFIG_SUBDIRS( X11 gtk qt servers )

AC_CONFIG_FILES([Makefile docs/Makefile perl/Makefile ])

AC_OUTPUT
