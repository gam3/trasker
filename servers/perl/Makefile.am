PERL=/usr/bin/perl

all-local:  Build
	./Build
	
check-local: Build
	./Build test

clean-local:
	rm -f Makefile.perl Makefile.perl.old Build
	rm -rf _build blib

EXTRA_DIST = Doxyfile Build.PL.in PerlDoxyfile README META.yml MANIFEST
EXTRA_DIST += lib t

Build: Build.PL
	$(PERL) $< ${BUILDFLAGS}

install-exec-local:
	./Build destdir=${DESTDIR} create_packlist=0 install


