# Makefile for ammc presentation slides
# Paolo Lucchesi
BUILDDIR := build

TEX := xelatex
TEX_FLAGS := --output-directory=$(BUILDDIR)

slides.pdf: slides.tex
	$(TEX) $(TEX_FLAGS) slides
	mv $(BUILDDIR)/$@ ./

all: slides.pdf ;

clean:
	-rm $(BUILDDIR)/*

.PHONY: all clean slides.pdf
