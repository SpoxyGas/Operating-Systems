#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handler(int signo) {
    (void)signo;
    printf("caught SIGINT\n");
}

int main() {

    struct sigaction act;

    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, NULL);

    while (1)
        pause();
}