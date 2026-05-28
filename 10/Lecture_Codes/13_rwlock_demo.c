#define _XOPEN_SOURCE 700
#include <pthread.h>
#include <stdio.h>

int shared_data = 0;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void* reader(void* arg) {
    (void)arg;
    pthread_rwlock_rdlock(&rwlock);
    printf("Reader %ld sees %d\n", (long)arg, shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void* writer(void* arg) {
    (void)arg;
    pthread_rwlock_wrlock(&rwlock);
    printf("Writer updates %d -> %d\n", shared_data, shared_data + 1);
    shared_data++;
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

int main() {
    pthread_t r1, r2, w;

    pthread_create(&r1, NULL, reader, (void*)1);
    pthread_create(&r2, NULL, reader, (void*)2);
    pthread_create(&w, NULL, writer, NULL);

    pthread_join(r1, NULL);
    pthread_join(r2, NULL);
    pthread_join(w, NULL);

    return 0;
}
