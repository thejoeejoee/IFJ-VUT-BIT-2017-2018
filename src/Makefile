CC=gcc

CFLAGS+=-std=c99 -Wall -Wextra -pedantic -O3 -DNDEBUG

TARGETS=ifj2017

RELEASE_ARCHIVE_NAME="xkobel02.tgz"
RELEASE_TMP_DIR:=$(shell mktemp -d)
SOURCES:=$(filter-out memory_debug.c, $(wildcard *.c))
OBJECTS=${SOURCES:.c=.o}

FILE_HEADER=/**\n\
	@project IFJ-VUT-BIT-2017-2018\n\
	@brief Compilator from IFJ2017 to IFJ2017Code three-address code.\n\
	@authors AUTHORS\n\
*/\n\n

all: $(TARGETS)

$(TARGETS): $(OBJECTS)

# DEV RULES
.PHONY: clean pack

clean:
	rm -f *.o Makefile.deps $(TARGETS)

pack: *.c *.h Makefile
	tar -cvf $(RELEASE_ARCHIVE_NAME) --files-from /dev/null;
	@for FILENAME in $^; do       											\
		AUTHORS="`git log --pretty=format:'%an' --follow $$FILENAME | sort | uniq | awk '{ printf "%s, ", $$0 }' `"; \
		cp $$FILENAME $(RELEASE_TMP_DIR)/$$FILENAME;        				\
		sed -i '1s;^;$(FILE_HEADER);' $(RELEASE_TMP_DIR)/$$FILENAME;		\
		sed -i -e "s/AUTHORS/$$AUTHORS/g" $(RELEASE_TMP_DIR)/$$FILENAME;	\
		cat $(RELEASE_TMP_DIR)/$$FILENAME;	                  				\
		tar -r $(RELEASE_ARCHIVE_NAME) $$FILENAME;						\
    done

# Makefile DEPS
Makefile.deps: *.h *.c
	$(CC) $(CFLAGS) -MM *.h $(SOURCES) > Makefile.deps

-include Makefile.deps
