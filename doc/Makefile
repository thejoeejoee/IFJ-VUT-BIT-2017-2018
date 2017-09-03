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
build/%.pdf: $(SOURCE_DIR)/%.tex
	mkdir $(TARGER_DIR)
	inkscape --file=./$(SOURCE_DIR)/$(SOURCE_MEDIA_DIR)/fit_logo.svg --export-area-drawing --without-gui --export-pdf=$(TARGER_DIR)/output.pdf
	inkscape --file=./$(SOURCE_DIR)/$(SOURCE_MEDIA_DIR)/data_structure.svg --export-area-drawing --without-gui --export-pdf=$(TARGER_DIR)/data_structure.pdf	
	pdflatex -halt-on-error -output-directory ./$(TARGER_DIR) ../$(SOURCE_DIR)/dokumentace.tex
	pdflatex -halt-on-error -output-directory ./$(TARGER_DIR) ../$(SOURCE_DIR)/dokumentace.tex

# Install important packages for latex build
install:
	- apt-get update && apt install  --no-install-recommends texlive-latex-base texlive-latex-extra texlive-fonts-recommended texlive-lang-czechslovak texlive-science texlive-latex-recommended

