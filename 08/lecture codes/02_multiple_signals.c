#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handler(int signo) {

    if (signo == SIGUSR1)
        printf("SIGUSR1 received\n");

    if (signo == SIGUSR2)
        printf("SIGUSR2 received\n");
}

int main() {

    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);

    printf("PID: %d\n", getpid());

    while (1)
        pause();
}