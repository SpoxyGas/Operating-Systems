CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

BIN = bin

PROGRAMS = \
	09_deadlock_example \
	10_deadlock_fixed_order \
	11_1_deadlock_trylock \
	11_2_deadlock_trylock \
	12_prodcons_semaphores \
	13_rwlock_demo \
	14_rwlock_two_writers \
	15_philosophers_deadlock \
	16_philosophers_reverse_order \
	17_barrier_demo \
	18_output_interleaving \
	19_thread_safe_output \
	20_pthread_once_demo

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