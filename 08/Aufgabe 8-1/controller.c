#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

int main (int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Error: Invalid number of arguments\n");
        fprintf(stderr, "Usage: %s <attack|shield|burst>\n", argv[0]);
        exit(1);
    }

    char *mode = argv[1];

    if (strcmp(mode, "attack") != 0 && strcmp(mode, "shield") != 0 && strcmp(mode, "burst") != 0) {
        fprintf(stderr, "Error: Invalid mode '%s'\n", mode);
        fprintf(stderr, "Usage: %s <attack|shield|burst>\n", argv[0]);
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "fork error");
        exit(1);
    }

    if (pid == 0) {
        if (strcmp(mode, "burst") == 0) {
            execl("./player", "player", "--auto-freeze", NULL);
        } else {
            execl("./player", "player", NULL);
        }
        fprintf(stderr, "execl error");
        exit(1);
    }

    printf("[CONTROLLER] Started player %d\n", pid);
    sleep(1);

    if (strcmp(mode, "attack") == 0) {
        printf("[CONTROLLER] Sending SIGUSR1 attack\n");
        kill(pid, SIGUSR1);
        sleep(1);
    }

    else if (strcmp(mode, "shield") == 0) {
        printf("[CONTROLLER] Sending SIGUSR2 shield\n");
        kill(pid, SIGUSR2);
        sleep(1);

        printf("[CONTROLLER] Sending SIGUSR1 attack\n");
        kill(pid, SIGUSR1);
        sleep(1);
    }

    else if (strcmp(mode, "burst") == 0) {
        printf("[CONTROLLER] Sending burst of 5 attacks\n");
        for (int i = 0; i < 5; i++) {
            kill(pid, SIGUSR1);
            usleep(10000);
        }
        sleep(6);
    } 

    kill(pid, SIGINT);
    waitpid(pid, NULL, 0);

    return 0;
}