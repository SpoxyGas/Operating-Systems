CC ?= gcc
CPPFLAGS ?= -D_POSIX_C_SOURCE=200809L
CFLAGS ?= -Wall -std=c11
LDLIBS ?= -lrt

BINARIES = matrix_mult matrix_worker

.PHONY: all clean

all: $(BINARIES)

matrix_mult: matrix_mult.o

matrix_worker: matrix_worker.o

clean:
	rm -f $(BINARIES) *.o
