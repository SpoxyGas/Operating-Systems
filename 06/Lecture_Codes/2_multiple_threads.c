#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

static void* threadfunction(void* arg) {
    long id = (long)arg;

    int delay = rand() % 100000;   // random delay up to 100000 microseconds
    usleep(delay);
    printf("Hello from thread %ld\n", id);
    return NULL;
}

int main(void) {
    const int N = 6;
    pthread_t t[N];

    for (long i = 0; i < N; i++) {
        pthread_create(&t[i], NULL, threadfunction, (void*)i);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(t[i], NULL);
    }

    puts("All done.");
    return 0;
}    
