#include <pthread.h>
#include <stdio.h>

#define N 5
pthread_mutex_t chopstick[N];

void* philosopher(void* arg) {
    int id = (long)arg;

    pthread_mutex_lock(&chopstick[id]);
    // usleep(1000);  // force overlap
    pthread_mutex_lock(&chopstick[(id + 1) % N]);

    printf("Philosopher %d eating\n", id);

    pthread_mutex_unlock(&chopstick[(id + 1) % N]);
    pthread_mutex_unlock(&chopstick[id]);

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


//gcc -Wall philosophers.c -o philosophers
//./philosophers
