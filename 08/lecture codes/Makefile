CC = gcc
CFLAGS = -Wall -Wextra -O2

BIN = bin

PROGRAMS = \
	01_basic_signal_handler \
	02_multiple_signals \
	03_signal_set \
	04_block_unblock_signal \
	05_sigaction_handler \
	06_eintr_read \
	07_alarm_timeout \
	08_signal_flag \
	09_sigchld_reap

TARGETS = $(PROGRAMS:%=$(BIN)/%)

all: $(BIN) $(TARGETS)

$(BIN):
	mkdir -p $(BIN)

$(BIN)/%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BIN)

run-all: all
	@for prog in $(TARGETS); do \
		echo "===== Running $$prog ====="; \
		$$prog; \
		echo ""; \
	done