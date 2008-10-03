#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([tasker],[0.0.1],[tasker-bugs@gam3.net])
AC_PREREQ(2.59)
AC_COPYRIGHT([Copyright (c) 2008 G. Allen Morris III])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

dnl See if we can find OpenSSL
dnl We can compile without OpenSSL if we have to
dnl ac_use_openssl="true"
dnl ac_openssl_lib_dir="/usr/lib /usr/local /usr/local/ssl /usr/local/ssl/lib /usr/pkg"
dnl ac_openssl_inc_dir="/usr/include /usr/local /usr/local/ssl /usr/pkg /usr/local/ssl/include"

AC_ARG_ENABLE(x11,
        AS_HELP_STRING([--disable-x11],[Disable building of X11 applications]),
        [case "${enableval}" in
                yes) HAVE_X11=yes ;;
                no)  HAVE_X11=no ;;
                *) AC_MSG_ERROR(bad value ${enableval} for --enable-x11) ;;
        esac],
        [HAVE_X11=yes])

AM_CONDITIONAL(HAVE_X11, test "x$HAVE_X11" = "xyes")

#
# Check for Qt 4
#
AC_ARG_ENABLE(qt4,
        AS_HELP_STRING([--disable-qt4],[Disable building of Qt4 applications]),
        [case "${enableval}" in
                yes) HAVE_QT4=yes ;;
                no)  HAVE_QT4=no ;;
                *) AC_MSG_ERROR(bad value ${enableval} for --enable-qt4) ;;
        esac],
        [HAVE_QT4=yes])

if test "x$HAVE_QT4" = "xyes" ; then
        PKG_CHECK_MODULES( QT4, [ QtCore >= 4.0.0 ])
        AC_SUBST(QT4_CFLAGS)
        AC_SUBST(QT4_LIBS)
            QT4_PREFIX="`$PKG_CONFIG --variable=prefix QtCore`/bin"
        MOC_QT4="no"
            AC_CHECK_FILE( "$QT4_PREFIX/moc-qt4", [ MOC_QT4=$QT4_PREFIX/moc-qt4 ], [
               AC_CHECK_FILE("$QT4_PREFIX/moc", [ MOC_QT4=$QT4_PREFIX/moc ], [
               AC_MSG_ERROR([Couldn't find QT4 moc])])])
        AC_SUBST(MOC_QT4)
fi
AM_CONDITIONAL(HAVE_QT4, test "x$HAVE_QT4" = "xyes")

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
