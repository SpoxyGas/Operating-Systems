// Do magic ╰( ͡° ͜ʖ ͡° )つ──☆*:・ﾟ and implement your "Hello world" programm here.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include "cthreadpool.h"

#define LINE_LENGTH 1024


struct task_info {
    const char *keyword;
    const char *filename;
    int file_index;
    int* results_array;
};


struct file_result {
    const char *filename;
    int matches;
};


void* worker_function(void* arg){
    struct task_info* task = (struct task_info*) arg;

    const char* keyword = task -> keyword;
    const char* filename = task -> filename;
    int file_index = task -> file_index;
    int* result_array = task -> results_array;

    char buffer[256];
    snprintf(buffer, sizeof(buffer),"TASK_START_%d", file_index);
    hidden_checkpoint(buffer);

    FILE* src = fopen(filename, "r");
    if(src == NULL){
        fprintf(stderr,"Error opening file %s\n", filename);
        result_array[file_index] = 0;

        snprintf(buffer, sizeof(buffer), "TASK_DONE_%d_MATCHES_0", file_index);
        hidden_checkpoint(buffer);

        free(arg);
        return NULL;
    }

    int count = 0;
    char text_buffer[LINE_LENGTH];
    while (fgets(text_buffer, sizeof(text_buffer), src) != NULL) {
        if (strstr(text_buffer, keyword) != NULL){
            count++;
        }
    }

    result_array[file_index] = count;
    fclose(src);

    snprintf(buffer, sizeof(buffer), "TASK_DONE_%d_MATCHES_%d", file_index, count);
    hidden_checkpoint(buffer);

    free(arg);
    return NULL;
}


int comparator(const void* a, const void* b){
    const struct file_result* fileA = (const struct file_result*) a;
    const struct file_result* fileB = (const struct file_result*) b;

    if (fileA->matches != fileB->matches) {
        return fileB->matches - fileA->matches; 
    }
    
    return (strcmp(fileA -> filename, fileB -> filename));
}


int main (int argc, char *argv[]){
    hidden_checkpoint("MAIN_START");

    if(argc < 4){
        fprintf(stderr, "Usage: ./parallel_grep.out <num_threads> <keyword> <file_name1> ... <file_nameN>\n");
        return EXIT_FAILURE;
    }

    int num_threads = atoi(argv[1]);
    if(num_threads <= 0){
        fprintf(stderr, "Error: Number of threads must be a positive integer.\n");
        return EXIT_FAILURE;
    }

    const char* keyword = argv[2];
    int num_files = argc - 3;
    int results_array[num_files];
    for(int i = 0; i < num_files; i++){
        results_array[i] = 0;
    }

    threadpool_t* pool = threadpool_t_init(num_threads);
    hidden_checkpoint("POOL_CREATED");

    for(int i = 0; i < num_files; i++){
        struct task_info* task = malloc(sizeof(struct task_info));
        if (task == NULL) { // Always good practice to check malloc
            fprintf(stderr, "Memory allocation failed\n");
            return EXIT_FAILURE;
        }

        task -> keyword = keyword;
        task -> filename = argv[i + 3];
        task -> file_index = i;
        task -> results_array = results_array;
        
        char buffer[256];
        snprintf(buffer, sizeof(buffer),"TASK_SUBMIT_%d", i);
        hidden_checkpoint(buffer);

        threadpool_add_work(pool, worker_function, task);
    }
    threadpool_wait(pool);
    hidden_checkpoint("POOL_DONE");
    threadpool_destroy(pool);

    int total_matches = 0;

    struct file_result sorted[num_files];
    for(int i = 0; i < num_files; i++){
        sorted[i].filename = argv[i + 3];
        sorted[i].matches = results_array[i];

        total_matches += results_array[i];
    }
    qsort(sorted, num_files, sizeof(struct file_result), comparator);

    printf("TOTAL_MATCHES %d\n", total_matches);
    printf("TOP5\n");

    int printed = 0;
    for (int i = 0; i < num_files && printed < 5; i++) {
        if (sorted[i].matches > 0) {
            printf("%s %d\n", sorted[i].filename, sorted[i].matches);
            printed++;
        }
    }

    return EXIT_SUCCESS;
}