PERL=/usr/bin/perl

noinst_DATA = .made_test

all-an: Build
	./Build

check: Build
	./Build test

Build: Build.PL
	$(PERL) $< ${BUILDFLAGS}

install-exec-am:
	./Build destdir=${DESTDIR} create_packlist=0 install

CLEANFILES =

clean-local:
	rm -f Makefile.perl Makefile.perl.old Build

#EXTRA_DIST = MANIFEST
EXTRA_DIST = lib

