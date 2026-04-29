#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *thread_func(void *arg) {
    char *buffer = malloc(64);
    if (buffer == NULL) return NULL;

    strcpy(buffer, " Hello world ! ");
    return buffer;
}

int main() {
    pthread_t thread;
    void *threadresult;

    pthread_create(&thread, NULL, thread_func, NULL);

    pthread_join(thread, &threadresult);

    char *message ;
    message = (char *)threadresult ;
    printf("I got %s back from the thread.\n", message);
    free(threadresult);

    return 0;
}