#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child
        printf("Child (%d): aborting!\n", getpid());
        abort();  // sends SIGABRT
    } else {
        // Parent
        int status;
        waitpid(pid, &status, 0);
        if (WIFSIGNALED(status)) {
            printf("Parent: child killed by signal %d\n", WTERMSIG(status));
        }
    }
    return 0;
}
