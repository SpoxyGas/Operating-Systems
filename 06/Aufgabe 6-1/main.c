// Do magic ╰( ͡° ͜ʖ ͡° )つ──☆*:・ﾟ and implement your "Hello world" programm here.

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "helper.h"
#include <time.h>


struct thread_info{
    int index;
    int points;
    int hits;
};


static double elapsed_seconds(struct timespec start, struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec)
         + (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
}


void* worker_function(void* arg) {
    struct thread_info *info = (struct thread_info *) arg;
    int i = info -> index;
    int points = info -> points;
    int k = 0;

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "T_START_%d", i);
    hidden_checkpoint(buffer);
   
    unsigned int seed = time(NULL) + i;
    for (int j = 0; j < points; j++){
        double x = (double) rand_r(&seed) / RAND_MAX;
        double y = (double) rand_r(&seed) / RAND_MAX;

        if((x * x) + (y * y) <= 1.0){
            k++;
        }
    }

    info -> hits = k;

    snprintf(buffer, sizeof(buffer), "T_DONE_%d_POINTS_%d_INSIDE_%d", i, points, k);
    hidden_checkpoint(buffer);

    return NULL;
}


int main (int argc, char *argv[]){
    hidden_checkpoint("MAIN_START");
    
    if(argc != 3){
        fprintf(stderr, "Usage: ./monte_carlo_pi.out <num_threads> <total_points>");
        return EXIT_FAILURE;
    }

    int num_threads = atoi(argv[1]);
    int total_points = atoi(argv[2]);

    pthread_t threads[num_threads];
    struct thread_info t_info[num_threads];

    int base_points = total_points / num_threads;
    int remainder = total_points % num_threads;

    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    
    for (int i = 0; i < num_threads; i++){
        t_info[i].index = i;

        if (i == num_threads - 1){
            t_info[i].points = base_points + remainder;
        }else{
            t_info[i].points = base_points;
        }

        if(pthread_create(&threads[i], NULL, worker_function, &t_info[i]) != 0){
            fprintf(stderr, "Thread creation failed");
            return EXIT_FAILURE;
        }
    }

    int total_hits = 0;
    for(int i = 0; i < num_threads; i++){
        pthread_join(threads[i], NULL);
        total_hits += t_info[i].hits;
    }

    double pi = 4.0 * total_hits / total_points;
    printf("Estimated pi = %f\n", pi);

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double elapsed = elapsed_seconds(t_start, t_end);
    printf("Execution time = %f\n", elapsed);

    return EXIT_SUCCESS;
}