# Bachelor degree thesis

This branch contains the LaTeX source code and compiled PDFs for my bachelor
degree thesis and slides for the AVR Multi Motor Control project presentation.

**Relator**: Prof. Giorgio Grisetti
**Correlator**: Docs. Barbara Bazzana
**Evaluator**: Profs. Silvia Bonomi

The thesis is presented in the degree session of _March 2022_.

# Building

### Requirements
All the LaTeX source code in this branch can be compiled using the _GNU Make_
build system.

Building the thesis requires `xelatex` and `bibtex`.

Building the presentation slides requires `xelatex`.

A full _TeXLive_ installation is recommended.

### GNU Make recipes

**Command** | **Description**
:-:|---
`make thesis`       | Builds the thesis PDF with `xelatex`
`make bibliography` | Builds the bibliography with `bibtex`
`make slides`       | Builds the presentation slides with `xelatex`
`make`              | Alias for `make thesis`
`make clean`        | Removes all the build auxiliary files

For a complete thesis build (with correct references and bibliography), run the
following commands:

```
make thesis
make bibliography
make thesis
make thesis
```

### Target files
The thesis can be found under the main directory of this branch as `thesis.pdf`

The presentation slides can be found under the `presentation/` directory as
`slides.pdf`
