#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int ready = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* consumer(void* arg) {
    (void)arg;
    pthread_mutex_lock(&m);

    printf("Consumer: checking condition\n");

    while (ready == 0) {
        printf("Consumer: waiting...\n");
        pthread_cond_wait(&cond, &m);
    }

    printf("Consumer: condition met, proceeding\n");

    pthread_mutex_unlock(&m);
    return NULL;
}

void* producer(void* arg) {
    (void)arg;
    sleep(1);  // ensure consumer waits first

    pthread_mutex_lock(&m);

    printf("Producer: updating condition\n");
    ready = 1;

    printf("Producer: signaling condition\n");
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&m);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, consumer, NULL);
    pthread_create(&t2, NULL, producer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}