#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int mysystem(const char *command) {
    pid_t pid = fork();

    if (pid < 0){
        perror("Fork error");
        return -1;
    }

    if (pid == 0){
        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        _exit(127);
    }

    else {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}


int main(int argc, char **argv) {
    if (argc != 2) return 2;
    int rc = mysystem(argv[1]);
    printf("%d\n", rc);
    return 0;
}