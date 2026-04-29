.PHONY: all clean FORCE

#############################################################
# DON'T change the variable names of INCLUDEDIRS and SOURCE
#############################################################
INCLUDEDIRS =
SOURCE = main.c helper.c

CC = gcc
CFLAGS = -pthread -Wall -Wextra -Wpedantic -std=c11 -O0 -g

all: monte_carlo_pi

monte_carlo_pi: FORCE
	$(CC) $(CFLAGS) $(INCLUDEDIRS:%=-I%) $(SOURCE) -o $@.out

clean:
	rm -f monte_carlo_pi.out

FORCE: ;
