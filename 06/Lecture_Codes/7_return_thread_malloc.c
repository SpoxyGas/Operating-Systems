#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *thread_func(void *arg) {
    long id = (long)arg;

    char *buffer = malloc(64);
    sprintf(buffer, "Hello from thread %ld", id);

    return buffer;
}

int main() {
    pthread_t thread1, thread2;
    void *threadresult1, *threadresult2;

    pthread_create(&thread1, NULL, thread_func, (void *)1);
    pthread_create(&thread2, NULL, thread_func, (void *)2);

    pthread_join(thread1, &threadresult1);
    pthread_join(thread2, &threadresult2);

    printf("%s\n", (char *)threadresult1);
    printf("%s\n", (char *)threadresult2);

    free(threadresult1);
    free(threadresult2);

    return 0;
}