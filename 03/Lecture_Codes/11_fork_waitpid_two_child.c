#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    pid_t pid1, pid2;
    int status;

    pid1 = fork();

    if (pid1 < 0) {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0) {
        // First child
        printf("I'm the first child %d\n", getpid());
        sleep(2);
        return 11;
    }

    // Only parent reaches here
    pid2 = fork();

    if (pid2 < 0) {
        perror("fork");
        exit(1);
    }

    if (pid2 == 0) {
        // Second child
        printf("I'm the second child %d\n", getpid());
        sleep(1);
        return 22;
    }

    // Parent
    printf("My first child is called %d\n", pid1);
    printf("My second child is called %d\n", pid2);

    // Wait for the FIRST child only
    waitpid(pid1, &status, 0);

    if (WIFEXITED(status)) {
        printf("First child exited with status %d\n", WEXITSTATUS(status));
    }

    printf("Parent waited for first child only\n");

    printf("This is the end (%d)\n", getpid());
    return 0;
}
