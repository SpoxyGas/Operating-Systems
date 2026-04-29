#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 1000

void* thread_func(void *arg) {
    return NULL;
}

double time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {

    pthread_t thread;

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for(int i = 0; i < N; i++) {
        pthread_create(&thread, NULL, thread_func, NULL);
        pthread_join(thread, NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("Thread creation time for %d threads: %f seconds\n",
           N, time_diff(start,end));

    return 0;
}
