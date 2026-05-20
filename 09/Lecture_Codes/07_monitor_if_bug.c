#include <pthread.h>
#include <stdio.h>

#define N 5

int buffer[N];
int count = 0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void put(int x) {
    pthread_mutex_lock(&m);

    if (count == N)
        pthread_cond_wait(&not_full, &m);

    buffer[count++] = x;

    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&m);
}

int get() {
    pthread_mutex_lock(&m);

    if (count == 0)
        pthread_cond_wait(&not_empty, &m);

    int x = buffer[--count];

    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&m);
    return x;
}

void* producer(void* arg) {
    (void)arg;
    for (int i = 1; i <= 5; i++) {
        put(i);
        printf("Produced %d\n", i);
    }
    return NULL;
}

void* consumer(void* arg) {
    (void)arg;
    for (int i = 1; i <= 5; i++) {
        int x = get();
        printf("Consumed %d\n", x);
    }
    return NULL;
}

int main() {
    pthread_t p, c;

    pthread_create(&p, NULL, producer, NULL);
    pthread_create(&c, NULL, consumer, NULL);

    pthread_join(p, NULL);
    pthread_join(c, NULL);

    return 0;
}