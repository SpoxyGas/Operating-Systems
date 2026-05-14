.PHONY: all clean FORCE

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O0 -g

all: controller player

controller: FORCE
	$(CC) $(CFLAGS) controller.c -o controller

player: FORCE
	$(CC) $(CFLAGS) player.c -o player

clean:
	rm -f controller player

FORCE: ;