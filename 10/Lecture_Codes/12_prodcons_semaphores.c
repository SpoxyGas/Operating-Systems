#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define N 5
#define ITEMS 6

int buffer[N], in = 0, out = 0;
sem_t empty_slots, full_slots;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void* producer(void* arg) {
    (void)arg;
    for (int item = 1; item <= ITEMS; item++) {
        sem_wait(&empty_slots);

        pthread_mutex_lock(&m);
        buffer[in] = item;
        in = (in + 1) % N;
        printf("Produced %d\n", item);
        pthread_mutex_unlock(&m);

        sem_post(&full_slots);
    }
    return NULL;
}

void* consumer(void* arg) {
    (void)arg;
    for (int i = 0; i < ITEMS; i++) {
        sem_wait(&full_slots);

        pthread_mutex_lock(&m);
        int item = buffer[out];
        out = (out + 1) % N;
        printf("Consumed %d\n", item);
        pthread_mutex_unlock(&m);

        sem_post(&empty_slots);
    }
    return NULL;
}

int main() {
    pthread_t p, c;

    sem_init(&empty_slots, 0, N);
    sem_init(&full_slots, 0, 0);

    pthread_create(&p, NULL, producer, NULL);
    pthread_create(&c, NULL, consumer, NULL);

    pthread_join(p, NULL);
    pthread_join(c, NULL);

    return 0;
}
