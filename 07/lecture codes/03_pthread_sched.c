#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <string.h>

void* work(void* arg) {
    (void)arg;
    int policy;
    struct sched_param param;

    if (pthread_getschedparam(pthread_self(), &policy, &param) == 0) {
        if (policy == SCHED_FIFO)
            printf("Thread policy: SCHED_FIFO, priority: %d\n", param.sched_priority);
        else if (policy == SCHED_RR)
            printf("Thread policy: SCHED_RR, priority: %d\n", param.sched_priority);
        else
            printf("Thread policy: OTHER, priority: %d\n", param.sched_priority);
    }

    volatile unsigned long long x = 0;
    while (1) {
        x++;
    }
    return NULL;
}

int main() {
    pthread_t t;
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);

    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    param.sched_priority = 50;
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    int ret = pthread_create(&t, &attr, work, NULL);
    if (ret != 0) {
        printf("pthread_create failed: %s\n", strerror(ret));
        return 1;
    }

    pthread_join(t, NULL);
    return 0;
}