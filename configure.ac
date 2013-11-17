#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([trasker],[0.0.10],[trasker-bugs@gam3.net])
AC_PREREQ(2.61)
AC_COPYRIGHT([Copyright (c) 2013 G. Allen Morris III])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS(config.h)

#
# Check for perl
#
AC_ARG_WITH(perl,
        AS_HELP_STRING([--without-perl],[Disable building of perl applications]),
        [with_perl=no],
	[with_perl=yes])

AM_CONDITIONAL(HAVE_PERL, test "x$with_perl" = "xyes")

if test "x$with_perl" = "xyes" ; then
   AC_CHECK_PROG(PERL, [perl], [perl])
   AC_PATH_PROG(PERL_PATH, [perl], [/usr/bin/perl])

   if test "x$prefix" != xNONE; then
     my_ac_prefix=$prefix
   else
     my_ac_prefix=$ac_default_prefix
   fi
   if echo $my_ac_prefix | grep "^/usr/local"  > /dev/null ; then
       pkgvdatadir=$(perl -MConfig -e 'print "$Config{installsitelib}"')
   else
       pkgvdatadir=$(perl -MConfig -e 'print "$Config{installvendorlib}"')
   fi

   AC_SUBST(PERL_PATH)

   AC_SUBST(pkgvdatadir)
fi

AC_ARG_VAR([BUILDFLAGS], [Flags passed to Build.PL])

#
# Check for X11
#
AC_ARG_WITH(x11,
        AS_HELP_STRING([--without-x11],[Disable building of X11 applications @<:@default=check@:>@]),
        with_x11=no,
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
        PKG_CHECK_MODULES([QT4], [ QtCore >= 4.0.0 ])
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

AC_ARG_ENABLE(debug, [AS_HELP_STRING([--enable-debug],[compile with debugging support])], enable_debug=yes, enable_debug=no)

if test "x$enable_debug" = "xyes" ; then
    AC_DEFINE(DEBUG, 1, [Define if debugging is enabled.])
fi

#echo HERE

if test "x$with_perl" = "xyes" ; then
    AC_CONFIG_FILES( servers/perl/Makefile.PL )
    AC_CONFIG_FILES( servers/perl/Makefile )
    AC_CONFIG_FILES( servers/perl/script/traskerd )
    AC_CONFIG_FILES( servers/perl/script/traskers )
    AC_CONFIG_FILES( servers/perl/lib/Trasker.pm )
    AC_CONFIG_FILES( clients/perl/Makefile.PL )
    AC_CONFIG_FILES( clients/perl/Makefile )
    AC_CONFIG_FILES( clients/perl/script/tnote )
    AC_CONFIG_FILES( clients/perl/script/trasker )
fi

if test "x$with_x11" = "xyes" ; then
    AC_PROG_CC
fi

if test "x$with_qt4" = "xyes" ; then
    AC_CONFIG_FILES( clients/qt/tlist/Makefile )
    AC_CONFIG_FILES( clients/qt/tlist/tlist.man )
    AC_CONFIG_FILES( clients/qt/tlist/tlist.ggo )
    AC_CONFIG_FILES( clients/qt/ttcp/Makefile )
#    AC_CONFIG_FILES( clients/qt/ttcp/ttcp.man )
fi

AC_CONFIG_FILES([
    Makefile
    docs/Makefile
])
AC_CONFIG_FILES( clients/x11/configure.ac )
AC_CONFIG_SUBDIRS( [clients/x11/] )


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

