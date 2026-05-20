#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define ITERATIONS 30

int counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* worker(void* arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&lock);

        int tmp = counter;   // read shared value
        usleep(50);          // same delay as race version
        tmp = tmp + 1;       // modify
        counter = tmp;       // write back
        usleep(100);

        pthread_mutex_unlock(&lock);
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
