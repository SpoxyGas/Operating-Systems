// Do magic ╰( ͡° ͜ʖ ͡° )つ──☆*:・ﾟ and implement your "Hello world" programm here.

#define _POSIX_C_SOURCE 200809L // fix for getline()
#define _DEFAULT_SOURCE         // fix for usleep()

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include "helper.h"


struct task_info{
    int mode;
    const char* input_file;
    const char* output_file;
};


static ssize_t safe_getline(char **lineptr, size_t *n, FILE *stream) {
    int old_state;
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
    ssize_t rc = getline(lineptr, n, stream);
    pthread_setcancelstate(old_state, NULL);
    return rc;
}


static int safe_fputs_flush(const char *s, FILE *stream) {
    int old_state;
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);

    int rc1 = fputs(s, stream);
    int rc2 = fflush(stream);

    pthread_setcancelstate(old_state, NULL);

    if (rc1 == EOF || rc2 == EOF) {
        return EOF;
    }
    return 0;
}


static void simulate_processing_work(void) {
    volatile unsigned long acc = 0;
    for (unsigned long i = 0; i < 50000000UL; i++) {
        acc += i;
    }
    (void)acc;
}


void* worker_function(void* arg){
    struct task_info* task = (struct task_info*)arg;
    int mode = task -> mode;
    const char* input = task -> input_file;
    const char* output = task -> output_file;

    hidden_checkpoint("WORKER_START");

    printf("WORKER START\n");
    fflush(stdout);

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    if(mode == 0){
        pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
        hidden_checkpoint("MODE_DEFERRED");
    } else {
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
        hidden_checkpoint("MODE_ASYNC");
    }
    
    FILE* in = fopen(input, "r");
    if(in == NULL){
        fprintf(stderr,"Error opening file %s\n", input);
        return NULL;
    }
    FILE* out = fopen(output, "w");
    if(out == NULL){
        fprintf(stderr,"Error opening file %s\n", output);
        return NULL;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_index = 0;

    while((read = safe_getline(&line, &len, in)) != -1){

        char buffer[256];
        snprintf(buffer, sizeof(buffer), "LINE_BEGIN_%d", line_index);
        hidden_checkpoint(buffer);

        simulate_processing_work();
        for(int i = 0; i < read; i++){
            line[i] = toupper(line[i]);
        }
        safe_fputs_flush(line, out);

        snprintf(buffer, sizeof(buffer), "LINE_END_%d", line_index);
        hidden_checkpoint(buffer);

        line_index++;
        if (mode == 0){
            pthread_testcancel();
        }
    }

    fclose(in);
    fclose(out);
    free(task);
    free(line);
    return NULL;
}


int main (int argc, char* argv[]){
    hidden_checkpoint("MAIN_START");

    if(argc != 4){
        fprintf(stderr, "Usage: ./file_processor.out <mode> <input_file> <output_file>\n");
        return EXIT_FAILURE;
    }

    int mode = atoi(argv[1]);
    if(mode != 0 && mode != 1){
        fprintf(stderr, "Mode has to be 0 or 1!\n");
        return EXIT_FAILURE;
    }

    struct task_info* task = malloc(sizeof(struct task_info));
    task->mode = mode;
    task->input_file = argv[2];
    task->output_file = argv[3];

    pthread_t thread;
    if (pthread_create(&thread, NULL, worker_function, task) != 0) {
        fprintf(stderr, "Failed to create thread\n");
        return EXIT_FAILURE;
    }
    
    usleep(100000);
    printf("CANCEL REQUEST\n");
    hidden_checkpoint("CANCEL_REQUEST");

    pthread_cancel(thread);

    void* return_value;
    pthread_join(thread, &return_value);

    if(return_value == PTHREAD_CANCELED){
        hidden_checkpoint("JOIN_CANCELED");
        printf("JOIN canceled\n");
    }else{
        hidden_checkpoint("JOIN_FINISHED");
        printf("JOIN finished\n");
    }

    return EXIT_SUCCESS;
}