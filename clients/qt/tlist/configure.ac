#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(tlist, [0.0.10], [gam3@gam3.net])
AC_CONFIG_SRCDIR([mainwindow.h])
AC_CONFIG_HEADER([config.h])

# AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB
AC_PROG_CXX

AC_PROG_MAKE_SET

ggl_host_type=linux

if test x$ggl_host_type = xlinux; then
  PKG_CHECK_MODULES(QT4, [QtGui >= 4.3 QtCore >= 4.3 QtNetwork >= 4.3],
		    [has_qt4=yes
		     MOC=`$PKG_CONFIG --variable=moc_location QtCore`;
		     UIC=`$PKG_CONFIG --variable=uic_location QtCore`],
		    [has_qt4=no])
  PKG_CHECK_MODULES(QT4LIBS, [QtGui >= 4.3 QtCore >= 4.3 QtNetwork >= 4.3 QtWebKit],
		    [has_qtwebkit=yes],
		    [has_qtwebkit=no])
elif test x$ggl_host_type = xmacosx; then
  # There is no pkgconfig for Qt on MacOSX, just do header check.
  AC_CHECK_HEADERS([QtCore/QtCore QtGui/QtGui QtNetwork/QtNetwork],
		   [has_qt4=yes], [has_qt4=no])
  AC_CHECK_HEADERS([QtNetwork/QtNetwork],
		   [has_qtnetwork=yes], [has_qtnetwork=no])
  AC_CHECK_HEADERS([QtWebKit/QtWebKit],
		   [has_qtwebkit=yes], [has_qtwebkit=no])
  # FIXME: Version should be checked. Only 4.4 and above are supported
  AC_CHECK_HEADERS([QtScript/QtScript],
		   [has_qtscript=yes], [has_qtscript=no])
  if test x$has_qt4 = xyes; then
    QT4_LIBS="-framework QtGui \
              -framework Carbon -framework AppKit -framework QtCore \
              -framework QtNetwork \
              -framework ApplicationServices -lz -lm"
    AC_PATH_PROG([MOC], [moc])
  fi
  if test x$has_qtnetwork = xyes; then
    QT4NETWORK_LIBS="-framework QtCore -framework QtNetwork -lz -lm"
  fi
  if test x$has_qtwebkit = xyes; then
    QT4WEBKIT_LIBS="-framework QtGui -framework Carbon -framework AppKit \
                    -framework QtCore -framework QtNetwork \
                    -framework QtWebKit -lz -lm"
  fi
  if test x$has_qtscript = xyes; then
    QT4SCRIPT_LIBS="-framework QtCore -framework QtScript -lz -lm"
  fi
else
  AC_MSG_WARN([Qt support is not available for $ggl_host_type])
fi

AC_SUBST(MOC)
AC_SUBST(UIC)

echo ${MOC} ${UIC}

CFLAGS="$CFLAGS $QT4_CFLAGS"

if test "x$MOC" = "x"; then
  AC_MSG_WARN([Can not find moc command, which is mandatory for Qt support])
  has_qt4=no
  has_qtnetwork=no
  has_qtwebkit=no
  has_qtscript=no
fi

# Checks for header files.
AC_PATH_X

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_HEADER_STDC

AC_CONFIG_FILES([Makefile tlist.ggo tlist.man])

AC_OUTPUT
