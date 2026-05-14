.PHONY: all clean run-norestart run-restart

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O0 -g

TARGET = task_runner
SOURCE = main.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)

run-norestart: $(TARGET)
	./$(TARGET) norestart

run-restart: $(TARGET)
	./$(TARGET) restart

clean:
	rm -f $(TARGET)