

TEX = latex

cobjects = $(patsubst %.data,%.tex,$(wildcard *.data))
dobjects = $(patsubst %.tex,%.dvi,$(wildcard *.tex))
pobjects = $(patsubst %.tex,%.pdf,$(wildcard *.tex))

all : $(dobjects) $(pobjects) $(cobjects)

%.tex : tex_invoice.pl
	perl -I ../lib ./tex_invoice.pl $*

%.pdf : %.tex
	pdflatex $<

%.tex : %.data makeinvoice.pl
	./makeinvoice.pl $<

clean :
	rm invoice-*.aux
	rm invoice-*.log
