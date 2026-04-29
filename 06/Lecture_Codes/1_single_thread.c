#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void *threadfunction(void *arg) {
    printf("%s\n", (char *)arg);
    return NULL;
}

int main() {
    pthread_t thread;
    char *msg = "Hello world!";
    int createerror = pthread_create(&thread, NULL, threadfunction, (void *) msg);
    pthread_join(thread, NULL);
    return 0;
}
