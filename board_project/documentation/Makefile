all: projet_sur_simavr.pdf

projet_sur_simavr.pdf: projet_sur_simavr.tex
	pdflatex projet_sur_simavr.tex  # deux fois pour les references
	pdflatex projet_sur_simavr.tex

clean:
	rm projet_sur_simavr.pdf *log *out *aux
