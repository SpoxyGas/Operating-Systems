#include <pthread.h>
#include <stdio.h>

int main() {
    pthread_attr_t attr;
    int scope;

    pthread_attr_init(&attr);

    pthread_attr_getscope(&attr, &scope);
    printf("Initial scope: %s\n",
           (scope == PTHREAD_SCOPE_SYSTEM) ? "SYSTEM" : "PROCESS");

    int ret = pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);

    if (ret != 0)
        printf("Setting PROCESS scope failed\n");

    pthread_attr_getscope(&attr, &scope);
    printf("Final scope: %s\n",
           (scope == PTHREAD_SCOPE_SYSTEM) ? "SYSTEM" : "PROCESS");

    return 0;
}