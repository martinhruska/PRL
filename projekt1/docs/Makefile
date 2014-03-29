# Autor: Martin Hruska
# E-mail: xhrusk16@stud.fit.vutbr.cz
# Datum: 14.3.2014
# Projekt: PRL

PROJ=doc
TEX_COMP=pdflatex # prekladac latexu
#DVI=dvips # prevod do post scriptu
#PDF=ps2pdf #prevod do pdf
#$A4=-sPAPERSIZE=a4
VLNA=vlna
VLNA_OPTS=-l -m -n

all:
	$(VLNA) $(VLNA_OPTS) $(PROJ).tex 
	$(TEX_COMP) $(PROJ).tex
	$(TEX_COMP) $(PROJ).tex

clean:
	rm *.pdf *.aux *.blg *.log *.dvi *.ps *.bbl
