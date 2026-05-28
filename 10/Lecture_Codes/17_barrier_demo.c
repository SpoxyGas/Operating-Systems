#define _XOPEN_SOURCE 700
#include <pthread.h>
#include <stdio.h>

#define THREADS 4
pthread_barrier_t barrier;

void* worker(void* arg) {
    long id = (long)arg;

    printf("Thread %ld reached barrier\n", id);
    pthread_barrier_wait(&barrier);
    printf("Thread %ld passed barrier\n", id);

    return NULL;
}

int main() {
    pthread_t t[THREADS];

    pthread_barrier_init(&barrier, NULL, THREADS);

    for (long i = 0; i < THREADS; i++)
        pthread_create(&t[i], NULL, worker, (void*)i);

    for (int i = 0; i < THREADS; i++)
        pthread_join(t[i], NULL);

    pthread_barrier_destroy(&barrier);
    return 0;
}
