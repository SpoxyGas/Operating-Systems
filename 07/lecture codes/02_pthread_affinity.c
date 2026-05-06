#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>

void* work(void* arg) {
    int core = *(int*)arg;
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);

    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    printf("Thread %lu bound to core %d\n",(unsigned long)pthread_self(), core);

    while (1) {
        int running_core = sched_getcpu();
        printf("Thread ID: %lu, assigned core: %d, running on CPU: %d\n",
               (unsigned long)pthread_self(), core, running_core);
        sleep(1);
    }

    return NULL;
}

int main() {
    pthread_t t1, t2;
    int core0 = 0, core1 = 1;

    pthread_create(&t1, NULL, work, &core0);
    pthread_create(&t2, NULL, work, &core1);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}