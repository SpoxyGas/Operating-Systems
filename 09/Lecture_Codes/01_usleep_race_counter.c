#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define ITERATIONS 30

int counter = 0;

void* worker(void* arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        int tmp = counter;   // read shared value
        usleep (50);
        tmp = tmp + 1;       // modify
        counter = tmp;       // write back
        usleep (100);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, worker, NULL);
    pthread_create(&t2, NULL, worker, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter: %d\n", counter);
    return 0;
}
