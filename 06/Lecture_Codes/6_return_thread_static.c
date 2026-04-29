#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void *thread_func(void *arg) {
    long id = (long)arg;
    // shared static buffer (one per process, NOT per thread)
    static char buffer[64];
    // write thread-specific message
    snprintf(buffer, sizeof(buffer), "Hello from thread %ld", id);
    // delay to increase chance of overwrite
    sleep(1);
    return buffer;
}

int main() {
    const int N = 3;
    pthread_t threads[N];
    void *results[N];

    // create threads
    for (long i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, thread_func, (void *)i);
    }

    // collect results
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], &results[i]);
    }

    // print results
    for (int i = 0; i < N; i++) {
        printf("Thread %d returned: %s (addr=%p)\n",
               i, (char *)results[i], results[i]);
    }

    return 0;
}