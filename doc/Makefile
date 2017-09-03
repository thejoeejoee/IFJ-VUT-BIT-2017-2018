# If there is file install, ignore it
.PHONY: install

# Source information
SOURCE_DIR = src
SOURCE_MEDIA_DIR = vector_media

# Target information
TARGER_DIR = build
TARGET_FILE = dokumentace.pdf


# Main translation line
all: clean $(TARGER_DIR)/$(TARGET_FILE)

# Clean translation directory 
clean:
	rm -fr $(TARGER_DIR)

# Translate from source code (latex template)
# to directory build
# TODO: more generic solution
build/%.pdf: $(SOURCE_DIR)/%.tex
	mkdir $(TARGER_DIR)
	inkscape --file=./$(SOURCE_DIR)/$(SOURCE_MEDIA_DIR)/fit_logo.svg --export-area-drawing --without-gui --export-pdf=$(TARGER_DIR)/output.pdf
	inkscape --file=./$(SOURCE_DIR)/$(SOURCE_MEDIA_DIR)/data_structure.svg --export-area-drawing --without-gui --export-pdf=$(TARGER_DIR)/data_structure.pdf	
	pdflatex -interaction nonstopmode -shell-escape -output-directory ./$(TARGER_DIR) ../$(SOURCE_DIR)/dokumentace.tex
	pdflatex -interaction nonstopmode -shell-escape -output-directory ./$(TARGER_DIR) ../$(SOURCE_DIR)/dokumentace.tex

# %.pdf: %.tex
#	(cd $(@D); for i in "$(seq 3)"; do pdflatex -interaction nonstopmode -shell-escape $(<F); done)

# Install important packages for latex build
install:
	- apt-get update && apt install  --no-install-recommends texlive-latex-base texlive-latex-extra texlive-fonts-recommended texlive-lang-czechslovak texlive-science texlive-latex-recommended

