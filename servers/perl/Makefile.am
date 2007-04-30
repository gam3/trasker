PERL=/usr/bin/perl
noinst_DATA =

man1_MANS = tasker_server.man

bin_SCRIPTS = tasker_server

check_SCRIPTS = check.pl

%.man : %
	pod2man $< > $@

CLEANFILES = $(bin_SCRIPTS) $(man1_MANS) $(check_SCRIPTS)
EXTRA_DIST = tasker_server.in

TESTS = check.pl

do_subst = sed -e 's,[@]datadir[@],$(datadir),g' \
               -e 's,[@]PERL[@],$(PERL),g' \
	       -e 's,[@]PACKAGE[@],$(PACKAGE),g' \
	       -e 's,[@]VERSION[@],$(VERSION),g'

tasker_server: tasker_server.in Makefile
	$(do_subst) < $(srcdir)/tasker_server.in > tasker_server
	chmod +x tasker_server

check.pl: Makefile
	echo perl -c tasker_server > check.pl
	chmod +x check.pl
