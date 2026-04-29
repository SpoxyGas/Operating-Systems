// Do magic ╰( ͡° ͜ʖ ͡° )つ──☆*:・ﾟ and implement your "Hello world" programm here.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <helper.h>

struct thread_info{
        int index;
        int points;
};

static double elapsed_seconds(struct timespec start, struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec)
         + (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

void* worker_function(void* arg) {
    struct thread_info* arg;
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
    

    for (size_t i = 0; i < num_threads; i++){
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
    

}