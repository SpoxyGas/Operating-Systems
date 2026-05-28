#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* worker(void* arg) {
    for (int i = 0; i < 5; i++) {
        printf("Thread %ld: part1 ", (long)arg);
        usleep(1000);   // force interleaving
        printf("part2 %d\n", i);
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
