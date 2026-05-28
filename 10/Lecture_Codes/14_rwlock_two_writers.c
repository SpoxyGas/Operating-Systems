#define _DEFAULT_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int shared_data = 0;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void* reader(void* arg) {
    pthread_rwlock_rdlock(&rwlock);
    printf("Reader %ld sees %d\n", (long)arg, shared_data);
    usleep(200000);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void* writer(void* arg) {
    long id = (long)arg;

    pthread_rwlock_wrlock(&rwlock);
    printf("Writer %ld entered\n", id);
    printf("Writer %ld updates %d -> %d\n", id, shared_data, shared_data + 1);
    usleep(500000);
    shared_data++;
    printf("Writer %ld leaving\n", id);
    pthread_rwlock_unlock(&rwlock);

    return NULL;
}

int main() {
    pthread_t r1, r2, w1, w2;

    pthread_create(&r1, NULL, reader, (void*)1);
    pthread_create(&r2, NULL, reader, (void*)2);
    pthread_create(&w1, NULL, writer, (void*)1);
    pthread_create(&w2, NULL, writer, (void*)2);

    pthread_join(r1, NULL);
    pthread_join(r2, NULL);
    pthread_join(w1, NULL);
    pthread_join(w2, NULL);

    return 0;
}