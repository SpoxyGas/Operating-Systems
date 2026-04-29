CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=gnu11 -g -lrt

SRC := $(wildcard *.c)
BIN_DIR := bin
TARGETS := $(patsubst %.c,$(BIN_DIR)/%,$(SRC))

.PHONY: all clean

all: $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%: %.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)
