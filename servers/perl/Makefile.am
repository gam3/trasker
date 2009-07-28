## Process this file with automake to create Makefile.in

## Copyright (C) 2008  G. Allen Morris III

## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.

## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
## 02110-1301, USA.

#SUBDIRS = tests

perllibdir = $(pkgvdatadir)/Trasker

bin_SCRIPTS = \
    scripts/traskerd \
    scripts/get_gaim \
    scripts/merge \
    scripts/traskers

dist_perllib_DATA = \
    lib/Trasker.pm \
    lib/Trasker/TTDB/Notes.pm \
    lib/Trasker/TTDB/Note.pm \
    lib/Trasker/TTDB/User.pm \
    lib/Trasker/TTDB/Auto.pm \
    lib/Trasker/TTDB/UserProjects.pm \
    lib/Trasker/TTDB/Times.pm \
    lib/Trasker/TTDB/TimeSlice.pm \
    lib/Trasker/TTDB/Project.pm \
    lib/Trasker/TTDB/Users.pm \
    lib/Trasker/TTDB/DBI.pm \
    lib/Trasker/TTDB/UserProject.pm \
    lib/Trasker/TTDB/Time.pm \
    lib/Trasker/TTDB/Projects.pm \
    lib/Trasker/TTDB.pm \
    lib/Trasker/Date.pm

nodist_perllib_DATA =

CLEANFILES = $(nodist_perllib_DATA)

## We can't use configure to do the substitution here; we must do it
## by hand.  We use a funny notation here to avoid configure
## substitutions in our text.
do_subst = in=`echo $@ | sed 's/\.[^.]*$$//'`; sed \
  -e 's,[@]APIVERSION[@],$(APIVERSION),g' \
  -e 's,[@]PACKAGE[@],$(PACKAGE),g' \
  -e 's,[@]PERL[@],$(PERL),g' \
  -e 's,[@]SHELL[@],$(SHELL),g' \
  -e 's,[@]VERSION[@],$(VERSION),g' \
  -e "s,[@]configure_input[@],Generated from $$in.in; do not edit by hand.,g" \
  -e 's,[@]datadir[@],$(datadir),g'

## This file depends on Makefile so it is rebuilt if $(VERSION),
## $(datadir) or other do_subst'ituted variables change.
## Use chmod a-w to prevent people from editing the wrong file by accident.
#Config.pm: Config.in Makefile
#        rm -f Config.tmp Config.pm
#        $(do_subst) $(srcdir)/Config.in >Config.tmp
#        chmod +x Config.tmp
#        chmod a-w Config.tmp
#        mv -f Config.tmp Config.pm

TESTS = \
   t/00setup.t

EXTRA_DIST = $(bin_SCRIPTS) $(TESTS)

XFAIL_TESTS = bob

