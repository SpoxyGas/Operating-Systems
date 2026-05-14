#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void child_handler(int signo) {
    (void)signo;
    int status;
    wait(&status);
    printf("child terminated\n");
}

int main() {
    signal(SIGCHLD, child_handler);
    pid_t pid = fork();
    if (pid == 0) {
        sleep(2);
        exit(0);
    }
    while (1)
        pause();
}