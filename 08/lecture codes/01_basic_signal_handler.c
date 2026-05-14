#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handler(int signo) {
    (void)signo;
    printf("Signal received\n");
}

int main() {
    signal(SIGINT, handler);
    while (1)
        pause();
}