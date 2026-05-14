#include <signal.h>
#include <stdio.h>

volatile sig_atomic_t got_sigint = 0;

void handler(int signo) {
    (void)signo;
    got_sigint = 1;
}

int main() {

    signal(SIGINT, handler);

    while (1) {
        if (got_sigint) {
            printf("SIGINT observed in main loop\n");
            got_sigint = 0;
        }
    }
}