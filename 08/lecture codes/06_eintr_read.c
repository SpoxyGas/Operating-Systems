#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

void handler(int signo) {
    (void)signo;
    printf("Signal received\n");
}

int main() {
    struct sigaction act;

    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;   // important: no SA_RESTART

    sigaction(SIGINT, &act, NULL);

    printf("Waiting for input...\n");

    char buf[100];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));

    if (n < 0 && errno == EINTR) {
        printf("read interrupted by signal\n");
    }

    printf("Program continues\n");
}