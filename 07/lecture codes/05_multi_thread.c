#include <pthread.h>
#include <stdio.h>

void* work(void* arg) {
    (void)arg;
    volatile unsigned long long x = 0;
    printf("Thread ID: %lu\n", (unsigned long)pthread_self());
    while (1) {
        x++;
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, work, NULL);
    pthread_create(&t2, NULL, work, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}