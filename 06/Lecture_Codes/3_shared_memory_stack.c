#include <pthread.h>
#include <stdio.h>

static int global_counter = 0;

static void* thread_func(void* arg) {
    int local = 42;
    printf("tid=%lu global@%p local@%p\n",
           (unsigned long)pthread_self(),
           (void*)&global_counter,
           (void*)&local);
    return NULL;
}

int main(void) {
    pthread_t a, b;
    pthread_create(&a, NULL, thread_func, NULL);
    pthread_create(&b, NULL, thread_func, NULL);
    pthread_join(a, NULL);
    pthread_join(b, NULL);
    return 0;
}