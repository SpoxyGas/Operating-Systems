CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

BIN = bin

PROGRAMS = \
	01_usleep_race_counter \
	02_busy_wait_demo \
	02_busy_wait_demo_with_usleep\
	03_mutex_counter \
	04_counting_sem_resource_pool \
	05_condvar_demo \
	06_bounded_buffer_monitor \
	07_monitor_if_bug \
	08_atomic_counter

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