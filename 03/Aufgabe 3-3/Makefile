.PHONY: all clean FORCE

#############################################################
# DON'T change the variable names of INCLUDEDIRS and SOURCE
#############################################################
INCLUDEDIRS =
SOURCE = main.c helper.c

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O0 -g

all: proc_mgmt

proc_mgmt: FORCE
	$(CC) $(CFLAGS) $(INCLUDEDIRS:%=-I%) $(SOURCE) -o $@.out

clean:
	rm -f proc_mgmt.out

FORCE: ;
