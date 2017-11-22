CC=gcc

CFLAGS+=-std=c99 -Wall -Wextra -pedantic -O3 -DNDEBUG -Wno-unused-parameter -Wno-variadic-macros
LDLIBS += -lm

TARGETS=ifj2017

SOURCES:=$(filter-out memory_debug.c, $(wildcard *.c))
OBJECTS=${SOURCES:.c=.o}

all: $(TARGETS)

$(TARGETS): $(OBJECTS)

# DEV RULES
.PHONY: clean

clean:
	rm -f *.o Makefile.deps $(TARGETS)

# Makefile DEPS
Makefile.deps: *.h *.c
	$(CC) $(CFLAGS) -MM *.h $(SOURCES) > Makefile.deps

-include Makefile.deps
