#include <pthread.h>
#include <stdio.h>

pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

void safe_print(long id, int i) {
    pthread_mutex_lock(&print_lock);
    printf("Thread %ld: line %d\n", id, i);
    pthread_mutex_unlock(&print_lock);
}

void* worker(void* arg) {
    for (int i = 0; i < 5; i++) {
        safe_print((long)arg, i);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, worker, (void*)1);
    pthread_create(&t2, NULL, worker, (void*)2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
