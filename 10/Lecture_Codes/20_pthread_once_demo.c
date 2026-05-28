#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_once_t once = PTHREAD_ONCE_INIT;

/* Initialization function: runs exactly once */
void init_database() {
    printf("Connecting to database...\n");
    sleep(1);  // simulate expensive setup
    printf("Database initialized\n");
}

void* worker(void* arg) {
    long id = (long)arg;

    /* Ensure initialization happens only once */
    pthread_once(&once, init_database);

    printf("Thread %ld using database\n", id);
    return NULL;
}

int main() {
    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, worker, (void*)1);
    pthread_create(&t2, NULL, worker, (void*)2);
    pthread_create(&t3, NULL, worker, (void*)3);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}