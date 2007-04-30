PERL=/usr/bin/perl
noinst_DATA =

man1_MANS = tasker_server.man

bin_SCRIPTS = tasker_server

#all : tasker_server.man

%.man : %
	pod2man $< > $@

CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = tasker_server.in

do_subst = sed -e 's,[@]datadir[@],$(datadir),g' \
               -e 's,[@]PERL[@],$(PERL),g' \
	       -e 's,[@]PACKAGE[@],$(PACKAGE),g' \
	       -e 's,[@]VERSION[@],$(VERSION),g'

tasker_server: tasker_server.in Makefile
	$(do_subst) < $(srcdir)/tasker_server.in > tasker_server
	chmod +x tasker_server


