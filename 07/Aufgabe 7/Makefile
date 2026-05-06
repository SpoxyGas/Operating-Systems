CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c11

TARGET = sched
SRC = simulator.c scheduler.c

all: $(TARGET)

$(TARGET): $(SRC) job.h
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean