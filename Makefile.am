
## Process this file with automake to produce Makefile.in

MAYBE_OPT =

if HAVE_X11
    MAYBE_OPT += X11
endif

if HAVE_QT4
    MAYBE_OPT += qt4/tlist
endif
 
SUBDIRS = perl docs ${MAYBE_OPT}

