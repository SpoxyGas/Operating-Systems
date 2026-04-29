#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cthreadpool.h"

typedef struct {
    int job_id;
    threadpool_t *tp;
} job_arg_t;

void *print_job(void *arg) {
    job_arg_t *a = (job_arg_t*)arg;

    usleep(50 * 1000);

    printf("Job %d executed on worker thread id=%d\n",
           a->job_id, get_thread_id(a->tp));

    free(a);
    return NULL;
}

int main(void) {
    threadpool_t *tp = threadpool_t_init(4);

    for (int i = 0; i < 10; i++) {
        job_arg_t *a = malloc(sizeof(*a));
        if (!a) exit(1);
        a->job_id = i;
        a->tp = tp;
        threadpool_add_work(tp, print_job, a);
    }

    threadpool_wait(tp);
    threadpool_destroy(tp);
    return 0;
}