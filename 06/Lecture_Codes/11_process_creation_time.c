#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define N 1000

double time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for(int i = 0; i < N; i++) {

        pid_t pid = fork();

        if(pid == 0) {
            exit(0);   // child exits immediately
        }
        else {
            wait(NULL);   // parent waits for child
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("Fork time for %d processes: %f seconds\n",
           N, time_diff(start,end));

    return 0;
}