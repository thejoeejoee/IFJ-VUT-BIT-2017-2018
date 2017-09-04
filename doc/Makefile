# If there is file install, ignore it
.PHONY: install all clean

# Source information
SOURCE_DIR = src
ASSETS_DIR = $(SOURCE_DIR)/assets

# Target information
DOC_NAME = doc
TARGET_DIR = build
TARGET_FILE = $(TARGET_DIR)/$(DOC_NAME).pdf


TEX = pdflatex -shell-escape

# figures = $(shell ls src/assets/*.svg | sed -e "s/svg/pdf/")

# Main translation line
all: $(TARGET_FILE)

$(TARGET_FILE): $(SOURCE_DIR)/$(DOC_NAME).tex
	mkdir -p $(TARGET_DIR) && while ($(TEX) -output-directory $(TARGET_DIR) $< ; grep -q "Rerun to get cross" $(TARGET_DIR)/$(DOC_NAME).log) do true ; done

# Install important packages for latex build
install:
	apt-get update && apt install --no-install-recommends texlive-latex-base texlive-latex-extra texlive-fonts-recommended texlive-lang-czechslovak texlive-science texlive-latex-recommended

clean:
	rm -fr $(TARGET_DIR) $(ASSETS_DIR)/*.pdf_tex $(ASSETS_DIR)/*.pdf
