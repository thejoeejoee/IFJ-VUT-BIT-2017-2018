CC=gcc

CFLAGS+=-std=c99 -Wall -Wextra -pedantic -O3

# TODO check
TARGETS=ifj2017

RELEASE_ARCHIVE_NAME="xkobel05.tgz"

all: $(TARGETS)


# DEV RULES
.PHONY: clean pack

clean:
	rm -f *.o Makefile.deps $(TARGETS)

pack: *.c *.h Makefile
	tar -cvzf $(RELEASE_ARCHIVE_NAME) $^

# Makefile DEPS
Makefile.deps: *.h *.c
	$(CC) $(CFLAGS) -MM *.h $(FILES_FOR_HTAB) > Makefile.deps

-include Makefile.deps
