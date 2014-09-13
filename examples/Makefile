SOURCES  = daemon.c
OBJECTS  = $(SOURCES:.c=.o)
BINARIES = $(OBJECTS:.o=)
CC       = gcc
CFLAGS   = -Wall -fPIC -O3 -g3
LIBASRC  = ../
LIBALIB  = -L$(LIBASRC) -static -la
LIBAINC  = -I$(LIBASRC)
INCLUDES = $(LIBAINC)
LDFLAGS  = $(LIBALIB) -lpthread

default: all
all: $(BINARIES)

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(BINARIES): $(OBJECTS)
	$(CC) $(@:=.o) $(LDFLAGS) -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(OBJECTS:.o=)
