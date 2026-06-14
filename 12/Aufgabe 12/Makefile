.PHONY: all run

all: fstree

#############################################################
# DON'T change the variable names of INCLUDEDIRS and SOURCE
#############################################################
# A list of include directories
INCLUDEDIRS =
# A list of source files
SOURCE = fstree.c

ifeq ($(origin CC),default)
CC = gcc
endif

# Warning flags
# For more information about gcc warnings: https://embeddedartistry.com/blog/2017/3/7/clang-weverything
WFLAGS = -Wall -Wextra -Wpedantic -g

# Compile without sanitizers and disable optimisation
# Changed -o $@.out to -o $@ so the executable is named exactly 'fstree'
fstree: FORCE
	$(CC) $(SOURCE) $(INCLUDEDIRS:%=-I%) -o $@ -std=c11 -O0 $(WFLAGS)

# Compile with address sanitizer enabled
asan: FORCE
	$(CC) $(SOURCE) $(INCLUDEDIRS:%=-I%) -o $@ -std=c11 -O0 $(WFLAGS) -Werror -fsanitize=address

# Compile with undefined behavior sanitizer enabled
ubsan: FORCE
	$(CC) $(SOURCE) $(INCLUDEDIRS:%=-I%) -o $@ -std=c11 -O0 $(WFLAGS) -Werror -fsanitize=undefined

# Compile with leak sanitizer enabled
lsan: FORCE
	$(CC) $(SOURCE) $(INCLUDEDIRS:%=-I%) -o $@ -std=c11 -O0 $(WFLAGS) -Werror -fsanitize=leak

# Compile with GCC static analysis enabled
staticAnalysis: FORCE
	$(CC) $(SOURCE) $(INCLUDEDIRS:%=-I%) -o $@ -std=c11 -O0 $(WFLAGS) -fanalyzer

# Execute the compiled program
# Changed from ./fstree.out to ./fstree
run:
	./fstree

# Make sure we always rebuild
# Required for the tester
FORCE: ;