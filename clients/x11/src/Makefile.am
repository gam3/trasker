
## Process this file with automake to produce Makefile.in

INCLUDES = $(X_CFLAGS) \
        -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
        -DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE=\"$(PACKAGE)\" \
	-DVERSION=\"$(VERSION)\"

INTLLIBS = -lX11 -lXt -lXmu -lXss

bin_PROGRAMS = winselect focus winraise

winselect_SOURCES = winselect.c
winraise_SOURCES = winraise.c
focus_SOURCES = focus.c

winselect_LDADD = $(INTLLIBS) $(X_LIBS)

winraise_LDADD = $(INTLLIBS) $(X_LIBS)

focus_LDADD = $(INTLLIBS) $(X_LIBS)

man1_MANS = winselect.man focus.man
EXTRA_DIST = $(man1_MANS)

common.o client.o : common.h

client : common.o client.o
	gcc -o client client.o common.o  -lssl -lcrypto -lpthread

