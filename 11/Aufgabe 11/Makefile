CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11

TARGET = pager

all: $(TARGET)

$(TARGET): pager.c
	$(CC) $(CFLAGS) pager.c -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean