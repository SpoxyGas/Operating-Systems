#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handler(int signo) {
    (void)signo;
    printf("SIGINT received\n");
}

int main() {

    sigset_t set;

    signal(SIGINT, handler);

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    sigprocmask(SIG_BLOCK, &set, NULL);

    printf("SIGINT blocked for 5 seconds\n");
    sleep(5);

    sigprocmask(SIG_UNBLOCK, &set, NULL);

    printf("SIGINT unblocked\n");

    sleep(5);
}