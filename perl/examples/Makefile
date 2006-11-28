

TEX = latex

cobjects = $(patsubst %.data,%.tex,$(wildcard *.data))
dobjects = $(patsubst %.tex,%.dvi,$(wildcard *.tex))
pobjects = $(patsubst %.tex,%.pdf,$(wildcard *.tex))

all : $(dobjects) $(pobjects) $(cobjects)

%.pdf : %.tex
	pdflatex $<

%.tex : %.data makeinvoice.pl
	./makeinvoice.pl $<
	${MAKE}

clean :
	rm invoice-*.aux
	rm invoice-*.log
