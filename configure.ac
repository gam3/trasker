#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([tasker],[0.0.1],[tasker-bugs@gam3.net])
AC_PREREQ(2.59)
AC_COPYRIGHT([Copyright (c) 2008 G. Allen Morris III])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

#
# Check for perl
#
AC_ARG_WITH(perl,
        AS_HELP_STRING([--without-perl],[Disable building of perl applications]),
        [with_perl=no],
	[with_perl=yes])

AM_CONDITIONAL(HAVE_PERL, test "x$with_perl" = "xyes")

AC_ARG_VAR([BUILDFLAGS], [Flags passed to Build.PL])

#
# Check for X11
#
AC_ARG_WITH(x11,
        AS_HELP_STRING([--without-x11],[Disable building of X11 applications @<:@default=check@:>@]),
        [with_x11=no],
        with_x11=yes)

AM_CONDITIONAL(HAVE_X11, test "x$with_x11" = "xyes")

#
# Check for Qt 4
#
AC_ARG_WITH(qt4,
        AS_HELP_STRING([--without-qt4],[Disable building of Qt4 applications]),
        [with_qt4=no],
        [with_qt4=yes])

if test "x$with_qt4" = "xyes" ; then
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
AM_CONDITIONAL(HAVE_QT4, test "x$with_qt4" = "xyes")

AC_ARG_ENABLE(debug, [AC_HELP_STRING([--enable-debug],
        [compile with debugging support])], enable_debug=yes, enable_debug=no)

if test "x$enable_debug" = "xyes" ; then
    AC_DEFINE(DEBUG, 1, [Define if debugging is enabled.])
fi

if test "x$with_perl" = "xyes" ; then
    AC_CONFIG_FILES( [ servers/perl/Makefile ] )
    AC_CONFIG_FILES( [ servers/perl/Build.PL ] )
    AC_CONFIG_FILES( [ servers/perl/scripts/taskerd ] )
fi
if test "x$with_x11" = "xyes" ; then
    AC_CONFIG_FILES( [ tasker-x11/configure.ac ] )
    AC_CONFIG_SUBDIRS( tasker-x11 )
fi
if test "x$with_qt4" = "xyes" ; then
    AC_CONFIG_SUBDIRS( clients/tlist )
fi

AC_CONFIG_FILES([ Makefile docs/Makefile ])

AC_OUTPUT

echo
echo $PACKAGE $VERSION

echo
echo Build Qt4 UI ................. : $with_qt4
echo Build for X11................. : $with_x11
echo Build for Perl ............... : $with_perl
echo
echo Print debugging messages...... : $enable_debug
echo
eval eval echo Tasker will be installed in $bindir.
echo
echo configure complete, now type \'make\'
echo

