#include <pthread.h>
#include <stdio.h>

// This method will fail.
void *thread_func(void *arg) {
    char buffer[64] = "Hello world!";
    return buffer;
}

int main() {
    pthread_t thread;
    int createerror = pthread_create(&thread, NULL, thread_func, NULL);
    void *threadresult;
    // Wait for the thread to terminate.
    pthread_join(thread, &threadresult);
    printf("Thread returned: %s\n", (char *)threadresult);    
    return 0;
}