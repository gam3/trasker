
## Process this file with automake to produce Makefile.in

INCLUDES = $(X_CFLAGS) \
        -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
        -DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE=\"$(PACKAGE)\" \
	-DVERSION=\"$(VERSION)\"

INTLLIBS = -lX11 -lXt -lXmu -lXss

bin_PROGRAMS = winselect focus

winselect_SOURCES = winselect.c
focus_SOURCES = focus.c

winselect_LDADD = $(INTLLIBS) $(X_LIBS)

focus_LDADD = $(INTLLIBS) $(X_LIBS)

man1_MANS = winselect.man focus.man
EXTRA_DIST = $(man1_MANS)
