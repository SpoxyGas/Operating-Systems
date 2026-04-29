CC ?= gcc
CPPFLAGS ?= -D_DEFAULT_SOURCE
CFLAGS ?= -Wall -Wextra -Werror -Wpedantic -std=c11
LDLIBS ?= -lrt

BINARIES = client proxy

.PHONY: all clean

all: $(BINARIES)

client: client.o

proxy: proxy.o

clean:
	rm -f $(BINARIES) *.o
