#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>

#define THREADS 4
#define ITERATIONS 200000

atomic_int counter = 0;

void* worker(void* arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        atomic_fetch_add(&counter, 1);
    }
    return NULL;
}

int main() {
    pthread_t t[THREADS];

    for (int i = 0; i < THREADS; i++)
        pthread_create(&t[i], NULL, worker, NULL);

    for (int i = 0; i < THREADS; i++)
        pthread_join(t[i], NULL);

    printf("Final counter: %d\n", counter);
    return 0;
}