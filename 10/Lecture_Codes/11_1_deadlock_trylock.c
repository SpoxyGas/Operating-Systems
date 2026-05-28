#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t B = PTHREAD_MUTEX_INITIALIZER;

void* t1(void* arg) {
    (void)arg;
    while (1) {
        printf("Thread 1: Locking A\n");
        pthread_mutex_lock(&A);

        printf("Thread 1: Trying B\n");
        if (pthread_mutex_trylock(&B) == 0)
            break;

        printf("Thread 1: Could not get B, releasing A\n");
        pthread_mutex_unlock(&A);
        usleep(100000);
    }

    printf("Thread 1: In critical section\n");

    pthread_mutex_unlock(&B);
    pthread_mutex_unlock(&A);
    printf("Thread 1: Released A and B\n");
    return NULL;
}

void* t2(void* arg) {
    (void)arg;
    while (1) {
        printf("Thread 2: Locking B\n");
        pthread_mutex_lock(&B);

        printf("Thread 2: Trying A\n");
        if (pthread_mutex_trylock(&A) == 0)
            break;

        printf("Thread 2: Could not get A, releasing B\n");
        pthread_mutex_unlock(&B);
        usleep(100000);
    }

    printf("Thread 2: In critical section\n");

    pthread_mutex_unlock(&A);
    pthread_mutex_unlock(&B);
    printf("Thread 2: Released B and A\n");
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, t1, NULL);
    pthread_create(&thread2, NULL, t2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Program finished normally\n");
    return 0;
}
