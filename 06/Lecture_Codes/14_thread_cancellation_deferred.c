#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

void *worker(void *arg) {
    printf("Worker (deferred): started\n");
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    while (1) {
        printf("Worker: working (no cancellation point)...\n");
        // Busy work (no cancellation point)
        for (volatile long i = 0; i < 10000000000; i++);
        printf("Worker: reaching the cancellation point\n");
        pthread_testcancel(); //<-- cancellation happens here (or we can use sleep(2);)
    }
    return NULL;
}

int main(void) {
    pthread_t tid;
    void *retval;
    pthread_create(&tid, NULL, worker, NULL);
    sleep(1);
    printf("Main: sending cancellation request (deferred)\n");
    pthread_cancel(tid);
    printf("Main: waiting...\n");
    pthread_join(tid, &retval);
    if (retval == PTHREAD_CANCELED) {
        printf("Main: thread canceled (deferred)\n");
    }
    return 0;
}
