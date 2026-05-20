#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define THREADS 5
#define RESOURCES 3

sem_t pool;
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

int active_threads = 0;

void* worker(void* arg) {
    long id = (long)arg;

    printf("T%ld is waiting for a resource\n", id);

    sem_wait(&pool);   // acquire one of the 3 available resources

    pthread_mutex_lock(&print_lock);
    active_threads++;
    printf("T%ld acquired a resource | active = %d\n", id, active_threads);
    pthread_mutex_unlock(&print_lock);

    sleep(2);          // simulate using the resource

    pthread_mutex_lock(&print_lock);
    active_threads--;
    printf("T%ld released a resource | active = %d\n", id, active_threads);
    pthread_mutex_unlock(&print_lock);

    sem_post(&pool);   // release the resource

    return NULL;
}

int main(void) {
    pthread_t t[THREADS];

    sem_init(&pool, 0, RESOURCES);

    for (long i = 1; i <= THREADS; i++) {
        pthread_create(&t[i - 1], NULL, worker, (void*)i);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(t[i], NULL);
    }

    sem_destroy(&pool);
    pthread_mutex_destroy(&print_lock);

    return 0;
}

