## Process this file with automake to produce Makefile.in

INCLUDES = $(X_CFLAGS) \
        -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
        -DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE=\"$(PACKAGE)\" \
	-DVERSION=\"$(VERSION)\" \
        -I /usr/include/qt3/

INTLLIBS = -lqt-mt

BUILT_SOURCES = tidle.moc

%.h: %.ui
	uic $< -o $@

%.cpp: %.ui
	uic -impl $*.h $< -o $@

bin_PROGRAMS = tidle 

tidle_SOURCES = tidle.cpp main.cpp

AM_CFLAGS = ${GLIB_CFLAGS}

tidle_LDADD = $(INTLLIBS) $(X_LIBS) $(GLIB_LIBS) -lqt-mt

tidle.moc : tidle.h
	moc tidle.h > tidle.moc
