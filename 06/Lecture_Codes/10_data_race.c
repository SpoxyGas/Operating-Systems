#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define THREADS 8
#define ITERS   1000000

static long counter = 0;

static void* worker(void* arg) {
    (void)arg;
    for (int i = 0; i < ITERS; i++) {
        counter++; // data race
    }
    return NULL;
}

int main(void) {
    pthread_t t[THREADS];

    for (int i = 0; i < THREADS; i++) pthread_create(&t[i], NULL, worker, NULL);
    for (int i = 0; i < THREADS; i++) pthread_join(t[i], NULL);

    printf("Expected: %ld\n", (long)THREADS * ITERS);
    printf("Actual:   %ld\n", counter);
    return 0;
}