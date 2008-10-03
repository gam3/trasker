## Process this file with automake to produce Makefile.in

.html.text:
	texi2html $<

all : tasker.info tasker.html tasker

info_TEXINFOS=tasker.texi

tasker_TEXINFOS=gpl.texi fdl.texi

EXTRA_DIST=tasker.html tasker.dvi

tasker : tasker.texi
	texi2html --split=chapter $<

tasker.html : tasker.texi
	texi2html $<
