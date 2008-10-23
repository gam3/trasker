
## Process this file with automake to produce Makefile.in

MAYBE_OPT =

if HAVE_X11
    MAYBE_OPT += tasker-x11
endif

if HAVE_QT4
    MAYBE_OPT += clients/tlist
endif

if HAVE_PERL
    MAYBE_OPT += servers
endif
 
SUBDIRS = docs ${MAYBE_OPT}


