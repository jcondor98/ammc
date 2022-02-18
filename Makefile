# Makefile for ammc thesis and presentation slides
# Paolo Lucchesi
.ONESHELL: # 'cd' command works in make recipes

SRCDIR := source
BUILDDIR := build

TEX := xelatex
TEX_FLAGS := --output-directory=../$(BUILDDIR)
BIBTEX := bibtex


thesis.pdf: $(wildcard $(SRCDIR)/main.*) $(wildcard $(SRCDIR)/chapters/*.tex) $(wildcard $(SRCDIR)/misc/*.tex)
	cd source
	$(TEX) $(TEX_FLAGS) main
	mv ../$(BUILDDIR)/main.pdf ../$@

thesis: thesis.pdf ;

bibliography:
	cp $(SRCDIR)/main.bib $(BUILDDIR)/
	cd $(BUILDDIR)
	$(BIBTEX) main

slides:
	$(MAKE) -C presentation/

all: thesis ;

clean:
	$(MAKE) -C presentation clean
	-rm $(BUILDDIR)/*

.PHONY: all clean slides thesis bibliography thesis.pdf
