CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = combat_queue
SRC = combat_queue.c

all: $(TARGET)

$(TARGET): $(SRC) combat_queue.h
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) actions.txt 5 3 2

clean:
	rm -f $(TARGET)

.PHONY: all run clean