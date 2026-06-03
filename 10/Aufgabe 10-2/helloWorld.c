// Do magic ╰( ͡° ͜ʖ ͡° )つ──☆*:・ﾟ and implement your "Hello world" programm here.

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "helper.h"

#define THREAD_COUNT 8

char winner = '\0';
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct thread_info{
    char** board;
    int condition; // 0-2 = rows top to bottom, 3-5 = columns left to right, 6-7 diagonal top to bottom and bottom to top
};

void* worker_function(void* arg){
    struct thread_info* info = (struct thread_info*) arg;
    char** board = info -> board;
    int condition = info -> condition;

    switch (condition){
        case 0:
            hidden_checkpoint("ROW_0");
            if (board[1][0] == board[1][1] && board[1][1] == board[1][2] && board[1][0] != ' ') {
                monitored_mutex_lock(&lock, "WINNER");
                winner = board[1][0];
                monitored_mutex_unlock(&lock, "WINNER");
            }
            break;

        case 1:
            hidden_checkpoint("ROW_1");
            if (board[2][0] == board[2][1] && board[2][1] == board[2][2] && board[2][0] != ' ') {
                monitored_mutex_lock(&lock, "WINNER");
                winner = board[2][0];
                monitored_mutex_unlock(&lock, "WINNER");
            }
            break;

        case 2:
            hidden_checkpoint("ROW_2");
            if (board[3][0] == board[3][1] && board[3][1] == board[3][2] && board[3][0] != ' ') {
                monitored_mutex_lock(&lock, "WINNER");
                winner = board[3][0];
                monitored_mutex_unlock(&lock, "WINNER");
            }
            break;

        case 3:
            hidden_checkpoint("COL_0");
            if (board[1][0] == board[2][0] && board[2][0] == board[3][0] && board[1][0] != ' ') {
                monitored_mutex_lock(&lock, "WINNER");
                winner = board[1][0];
                monitored_mutex_unlock(&lock, "WINNER");
            }
            break;

        case 4:
            hidden_checkpoint("COL_1");
            if (board[1][1] == board[2][1] && board[2][1] == board[3][1] && board[1][1] != ' ') {
                monitored_mutex_lock(&lock, "WINNER");
                winner = board[1][1];
                monitored_mutex_unlock(&lock, "WINNER");
            }
            break;

        case 5:
            hidden_checkpoint("COL_2");
            if (board[1][2] == board[2][2] && board[2][2] == board[3][2] && board[1][2] != ' ') {
                monitored_mutex_lock(&lock, "WINNER");
                winner = board[1][2];
                monitored_mutex_unlock(&lock, "WINNER");
            }
            break;

        case 6:
            hidden_checkpoint("DIAG_0");
            if (board[1][0] == board[2][1] && board[2][1] == board[3][2] && board[1][0] != ' ') {
                monitored_mutex_lock(&lock, "WINNER");
                winner = board[1][0];
                monitored_mutex_unlock(&lock, "WINNER");
            }
            break;

        case 7:
            hidden_checkpoint("DIAG_1");
            if (board[3][0] == board[2][1] && board[2][1] == board[1][2] && board[3][0] != ' ') {
                monitored_mutex_lock(&lock, "WINNER");
                winner = board[3][0];
                monitored_mutex_unlock(&lock, "WINNER");
            }
            break;
    }
    return NULL;
}

int main (int argc, char* argv[]){
    if (argc != 4){
        fprintf(stderr, "Invalid input");
        return EXIT_FAILURE;
    }

    hidden_checkpoint("MAIN_START");

    pthread_t threads[THREAD_COUNT];
    struct thread_info info[THREAD_COUNT];

    for(size_t i = 0; i < THREAD_COUNT; i++){
        info[i].board = argv;
        info[i].condition = i;

        if(pthread_create(&threads[i], NULL, worker_function, &info[i]) != 0){;
            fprintf(stderr, "Thread creation failed");
            return EXIT_FAILURE;
        }
    }

    for(size_t i = 0; i < THREAD_COUNT; i++){
        pthread_join(threads[i], NULL);
    }

    if(winner == 'X' || winner == 'O'){
        printf("Winner: %c\n", winner);
    } else {
        printf("No winner\n");
    }

    return EXIT_SUCCESS;
}