#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>

int main() {

    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    printf("Signal set created\n");

    return 0;
}