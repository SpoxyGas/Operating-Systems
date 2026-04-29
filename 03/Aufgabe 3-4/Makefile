.PHONY: all clean FORCE

INCLUDEDIRS =
SOURCE = main.c

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O0 -g

all: mysystem

mysystem: FORCE
	$(CC) $(CFLAGS) $(SOURCE) -o $@.out

clean:
	rm -f mysystem.out

FORCE: ;
