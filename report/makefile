all:
	@pdflatex main > /dev/null
	@biber main > /dev/null
	@pdflatex main > /dev/null
	@pdflatex main > /dev/null
	@python3 words.py main.tex
	@echo "Opening file..."
	@open main.pdf

simple:
	@pdflatex main > /dev/null
	@pdflatex main > /dev/null
	@python3 words.py main.tex
	@echo "Opening file..."
	@open main.pdf

count:
	@python3 words.py main.tex
