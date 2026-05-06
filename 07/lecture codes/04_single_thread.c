#include <pthread.h>
#include <stdio.h>
int main() {
    volatile unsigned long long x = 0;
    printf("Thread ID: %lu\n", (unsigned long)pthread_self());
    while (1) {
        x++;
    }
    return 0;
}