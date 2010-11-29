
## Process this file with automake to produce Makefile.in

MAYBE_OPT =

if HAVE_X11
    MAYBE_OPT += clients/x11
endif

if HAVE_QT4
    MAYBE_OPT += clients/qt/tlist
    MAYBE_OPT += clients/qt/ttcp
endif

if HAVE_PERL
    MAYBE_OPT += servers/perl
    MAYBE_OPT += clients/perl
endif
 
SUBDIRS = docs ${MAYBE_OPT}

