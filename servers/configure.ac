#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([tasker-server],[0.0.1],[tasker-bugs@gam3.net])
AC_PREREQ(2.59)
AC_COPYRIGHT([Copyright (c) 2008 G. Allen Morris III])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

dnl

AC_PROG_PERL_VERSION(5.6.0)
AC_PROG_PERL_MODULES( IO::Socket::SSL Params::Validate Net::TCPwrappers, ,AC_MSG_WARN(Need some Perl modules))

AC_ARG_VAR([BUILDFLAGS], [Flags passed to Build.PL])

AC_CONFIG_FILES([Makefile perl/Makefile perl/Build.PL perl/scripts/taskerd perl/scripts/taskers ])

AC_OUTPUT
