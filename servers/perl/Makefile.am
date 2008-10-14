PERL=/usr/bin/perl
noinst_DATA = .made_test

#.made_mod: Makefile.perl Build
#	$(MAKE) -fMakefile.perl
#	./Build
#	touch $@

.made_test: Build
	./Build test
	touch .made_test

#Makefile.perl: Makefile.PL
#	$(PERL) $< MAKEFILE=$@

Build: Build.PL
	$(PERL) $< MAKEFILE=$@

install-data-local:
	$(MAKE) -fMakefile.perl PREFIX=$(prefix) install

CLEANFILES = *~ .made_mod yMod.pm
clean-local:
	if test -f Makefile.perl; them $(MAKE) -fMakefile.perl realclean; fi
	rm -f Makefile.perl Makefile.perl.old



