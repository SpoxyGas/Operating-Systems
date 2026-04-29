#include <pthread.h>
#include <stdio.h>

void *thread_func(void *arg) {
    int code = 5;
    return (void *)code ;
}

int main() {
    pthread_t thread;
    int createerror = pthread_create(&thread, NULL, thread_func, NULL);
    void *threadresult;
    // Wait for the thread to terminate.
    pthread_join(thread, &threadresult);
    printf("Thread returned: %d\n", threadresult);    
    return 0;
}