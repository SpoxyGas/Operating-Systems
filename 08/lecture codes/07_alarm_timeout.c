#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void timeout_handler(int signo) {
    (void)signo;
    printf("timeout occurred\n");
    exit(1);
}

int main() {
    signal(SIGALRM, timeout_handler);
    alarm(5);
    printf("You have 5 seconds...\n");
    pause();
    return 0;
}