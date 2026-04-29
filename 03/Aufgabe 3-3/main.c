#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "helper.h"

#define NUM_CHILDREN 6

int main (void){
    hidden_checkpoint("PARENT_START");

    pid_t pid_array[NUM_CHILDREN];

    char buffer[100];

    for (int i = 0; i < NUM_CHILDREN; i++){
        pid_t pid = fork();
        
        if (pid < 0){
            perror("Fork failed");
            exit(1);
        }

        if (pid == 0) {
            snprintf(buffer, sizeof(buffer), "C_START_%d", i);
            hidden_checkpoint(buffer);
            sleep(i % 3);

            if (i == 2){
                hidden_checkpoint("C_ABORT_2");
                abort();
            } else {
                _exit(40 + i);
            }
        }

        else {
            pid_array[i] = pid;
            snprintf(buffer, sizeof(buffer), "P_FORK_%d_CHILD_%d", i, pid);
            hidden_checkpoint(buffer);
        }
    }

    int status;
    for (int i = 0; i < NUM_CHILDREN; i++){
        waitpid(pid_array[i], &status, 0);
        snprintf(buffer, sizeof(buffer), "P_REAP_%d", i);
        hidden_checkpoint(buffer);
    }

    pid_t orphan = fork();

    if (orphan < 0){
        perror("Orphan fork failed");
        exit(2);
    }

    if (orphan == 0){
        hidden_checkpoint("ORPHAN_BEFORE");
        sleep(2);
        hidden_checkpoint("ORPHAN_AFTER");
        exit(EXIT_SUCCESS);
    }

    hidden_checkpoint("PARENT_EXITING");
    sleep(1);
    return EXIT_SUCCESS;
}