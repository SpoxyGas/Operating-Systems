#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

volatile int ready = 0;  // shared flag

void* spinner(void* arg) {
    (void)arg;
    while (!ready) {
        // busy waiting
        usleep(1000);
    }
    printf("Spinner finished\n");
    return NULL;
}

void* setter(void* arg) {
    (void)arg;
    sleep(2);                // simulate work
    ready = 1;               // update condition
    printf("Setter set ready = 1\n");
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, spinner, NULL);
    pthread_create(&t2, NULL, setter, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}


