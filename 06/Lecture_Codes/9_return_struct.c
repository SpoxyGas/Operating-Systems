#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct thread_args {
    int a;
    double b;
};

struct thread_result {
    long x;
    double y;
};

void *thread_func(void *args) {
    struct thread_args * thread_args = args;
    struct thread_result *res = malloc(sizeof *res);
    res->x = 10 + thread_args->a;
    res->y = thread_args->a * thread_args->b;
    return res;
}

int main() {
    pthread_t threadL;
    struct thread_args in = {.a = 10, .b = 3.141592653};
    void *out_void;
    struct thread_result *out;
    pthread_create(&threadL, NULL, thread_func, &in);
    pthread_join(threadL, &out_void);
    out = out_void;
    printf("out -> x = %ld\tout -> b = %f\n", out->x, out->y);
    free(out);
    return 0;
}