## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = dejagnu

INCLUDES = $(X_CFLAGS) \
        -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
        -DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE=\"$(PACKAGE)\" \
	-DVERSION=\"$(VERSION)\" \
	$(QT_CFLAGS)

INTLLIBS = -lqt-mt

BUILT_SOURCES = tlist.moc addproject.moc notes.moc ttcp.moc auto_select.moc tselect_help.moc

bin_PROGRAMS = tlist

tlist_SOURCES = main.cpp tlist.cpp addproject.cpp ttcp.cpp tlist.h addproject.h \
 addproject.ui.h ttcp.h addproject.ui \
 notes.cpp notes.h notes.ui.h notes.ui global.h \
 auto_select.cpp auto_select.h auto_select.ui.h auto_select.ui \
 base64.cpp base64.h

AM_CFLAGS = ${GLIB_CFLAGS}

CLEANFILES = *.moc

MAINTAINERCLEANFILES = notes.h notes.cpp addproject.h addproject.cpp

tlist_LDADD = $(INTLLIBS) $(X_LIBS) $(GLIB_LIBS) $(QT_LIBS)

%.h: %.ui
	uic $< -o $@

%.cpp: %.ui %.h
	uic -impl $*.h $< -o $@

%.moc : %.h
	moc $< > $@

man1_MANS = tlist.man

dist_man1_MANS = tlist.man

