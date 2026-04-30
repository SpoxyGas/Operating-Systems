.PHONY: all clean FORCE

INCLUDEDIRS =
SOURCE = main.c helper.c

CC = gcc
CFLAGS = -pthread -Wall -Wextra -Wpedantic -std=c11 -O0 -g

all: file_processor

file_processor: FORCE
	$(CC) $(CFLAGS) $(INCLUDEDIRS:%=-I%) $(SOURCE) -o $@.out

clean:
	rm -f file_processor.out

FORCE: ;