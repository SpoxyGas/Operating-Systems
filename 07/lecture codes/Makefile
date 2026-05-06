CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
PTHREAD = -pthread

BIN_DIR = bin

TARGETS = \
	$(BIN_DIR)/01_cpu_task \
	$(BIN_DIR)/02_pthread_affinity \
	$(BIN_DIR)/03_pthread_sched \
	$(BIN_DIR)/04_single_thread \
	$(BIN_DIR)/05_multi_thread \
	$(BIN_DIR)/06_scope

all: $(BIN_DIR) $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/01_cpu_task: 01_cpu_task.c
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_DIR)/02_pthread_affinity: 02_pthread_affinity.c
	$(CC) $(CFLAGS) -o $@ $< $(PTHREAD)

$(BIN_DIR)/03_pthread_sched: 03_pthread_sched.c
	$(CC) $(CFLAGS) -o $@ $< $(PTHREAD)

$(BIN_DIR)/04_single_thread: 04_single_thread.c
	$(CC) $(CFLAGS) -o $@ $< $(PTHREAD)

$(BIN_DIR)/05_multi_thread: 05_multi_thread.c
	$(CC) $(CFLAGS) -o $@ $< $(PTHREAD)

$(BIN_DIR)/06_scope: 06_scope.c
	$(CC) $(CFLAGS) -o $@ $< $(PTHREAD)

clean:
	rm -rf $(BIN_DIR)