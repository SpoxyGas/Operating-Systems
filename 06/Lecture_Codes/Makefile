CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=gnu11 -g

BIN_DIR := bin

# Special cases
CTHREADPOOL_SRC := 13_cthreadpool_hello_jobs.c
NO_PTHREAD_SRC := 11_process_creation_time.c

# All ordinary source files in this directory
SRC := $(wildcard *.c)

# Exclude helper library and special-case sources from normal builds
NORMAL_SRC := $(filter-out cthreadpool.c $(CTHREADPOOL_SRC) $(NO_PTHREAD_SRC),$(SRC))

# Executables
NORMAL_TARGETS := $(patsubst %.c,$(BIN_DIR)/%,$(NORMAL_SRC))
NO_PTHREAD_TARGETS := $(patsubst %.c,$(BIN_DIR)/%,$(NO_PTHREAD_SRC))
CTHREADPOOL_TARGET := $(BIN_DIR)/$(basename $(CTHREADPOOL_SRC))

TARGETS := $(NORMAL_TARGETS) $(NO_PTHREAD_TARGETS) $(CTHREADPOOL_TARGET)

.PHONY: all clean

all: $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Ordinary programs: compile with pthread
$(BIN_DIR)/%: %.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ -pthread

# Special program: no pthread
$(BIN_DIR)/11_process_creation_time: 11_process_creation_time.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Special program: uses cthreadpool library
$(BIN_DIR)/13_cthreadpool_hello_jobs: 13_cthreadpool_hello_jobs.c cthreadpool.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -pthread

clean:
	rm -f $(TARGETS)