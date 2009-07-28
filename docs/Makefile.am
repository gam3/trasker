## Process this file with automake to produce Makefile.in

.html.text:
	texi2html $<

all : trasker.info trasker.html trasker

info_TEXINFOS=trasker.texi

trasker_TEXINFOS=gpl.texi fdl.texi

EXTRA_DIST=trasker.html trasker.dvi

trasker : trasker.texi
	texi2html --split=chapter $<

trasker.html : trasker.texi
	texi2html $<

CLEANFILES = trasker/*.html
