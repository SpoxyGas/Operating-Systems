#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *worker(void *arg) {
    printf("Worker (async): started\n");

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1) {
        printf("Worker: working...\n");

        // Same busy work
        for (volatile long i = 0; i < 10000000000; i++);

        printf("Worker: about to sleep\n");
        sleep(2);  // may never reach this
    }

    return NULL;
}

int main(void) {
    pthread_t tid;
    void *retval;

    pthread_create(&tid, NULL, worker, NULL);

    sleep(1);
    printf("Main: sending cancellation request (async)\n");
    pthread_cancel(tid);

    printf("Main: waiting...\n");
    pthread_join(tid, &retval);

    if (retval == PTHREAD_CANCELED) {
        printf("Main: thread canceled immediately (async)\n");
    }

    return 0;
}
