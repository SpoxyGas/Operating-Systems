#include <pthread.h>
#include <stdio.h>

#define N 5
pthread_mutex_t chopstick[N];

void* philosopher(void* arg) {
    int id = (long)arg;
    int left = id;
    int right = (id + 1) % N;

    if (id == N - 1) {
        // one philosopher uses reverse order
        pthread_mutex_lock(&chopstick[right]);
        pthread_mutex_lock(&chopstick[left]);
    } else {
        pthread_mutex_lock(&chopstick[left]);
        pthread_mutex_lock(&chopstick[right]);
    }

    printf("Philosopher %d eating\n", id);

    pthread_mutex_unlock(&chopstick[left]);
    pthread_mutex_unlock(&chopstick[right]);
    return NULL;
}

int main() {
    pthread_t t[N];

    for (int i = 0; i < N; i++)
        pthread_mutex_init(&chopstick[i], NULL);

    for (long i = 0; i < N; i++)
        pthread_create(&t[i], NULL, philosopher, (void*)i);

    for (int i = 0; i < N; i++)
        pthread_join(t[i], NULL);

    return 0;
}