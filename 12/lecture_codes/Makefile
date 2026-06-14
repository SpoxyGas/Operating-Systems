CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

BIN = bin

PROGRAMS = \
	01_stat \
	02_open_file \
	03_lseek \
	04_mmap \
	05_readdir_stat \
	06_no_lock \
	07_flock

TARGETS = $(PROGRAMS:%=$(BIN)/%)

.PHONY: all clean run-all

all: $(BIN) $(TARGETS)

$(BIN):
	mkdir -p $(BIN)

$(BIN)/%: %.c | $(BIN)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BIN)

run-all: all
	@for prog in $(TARGETS); do \
		echo "===== Running $$prog ====="; \
		./$$prog; \
		echo ""; \
	done