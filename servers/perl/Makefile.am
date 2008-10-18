PERL=/usr/bin/perl

noinst_DATA = .made_test

all-am: Build
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

EXTRA_DIST = lib scripts examples cgi-bin t
EXTRA_DIST += Doxyfile Build.PL Build.PL.in MANIFEST PerlDoxyfile README META.yml
