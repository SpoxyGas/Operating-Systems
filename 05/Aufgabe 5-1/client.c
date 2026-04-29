#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MSG_SIZE 256

static void print_usage(const char *program_name)
{
    fprintf(stderr, "Usage: %s <msg-queue-name> <prio> <command>\n", program_name);
}

static bool parse_priority(const char *text, unsigned int *value)
{
    char *endptr = NULL;
    unsigned long parsed = 0;

    if (text[0] == '-') {
        return false;
    }

    errno = 0;
    parsed = strtoul(text, &endptr, 10);
    if (errno != 0 || endptr == text || *endptr != '\0' || parsed > UINT_MAX) {
        return false;
    }

    *value = (unsigned int)parsed;
    return true;
}

static bool is_valid_queue_name(const char *queue_name)
{
    return queue_name[0] == '/' && queue_name[1] != '\0';
}

int main(int argc, char *argv[])
{
    const char *queue_name = NULL;
    const char *command = NULL;
    unsigned int priority = 0;

    if (argc != 4) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    queue_name = argv[1];
    command = argv[3];

    if (!is_valid_queue_name(queue_name)) {
        fprintf(stderr, "Error: <msg-queue-name> must start with '/'\n");
        return EXIT_FAILURE;
    }

    if (!parse_priority(argv[2], &priority)) {
        fprintf(stderr, "Error: <prio> must be a non-negative integer\n");
        return EXIT_FAILURE;
    }

    (void)command;


    mqd_t mq = mq_open(queue_name, O_WRONLY);
    if(mq == (mqd_t) - 1){
        fprintf(stderr, "Error: Failed to open message queue\n");
        return EXIT_FAILURE;
    }
    int p1[2];
    if(pipe(p1)){
        perror("Error opening pipe");
        return EXIT_FAILURE;
    }

    pid_t pidChild = fork();
    if (pidChild == -1) {
        perror("Error forking");
        return EXIT_FAILURE;
    }

    if(pidChild == 0){
        dup2(p1[1], STDOUT_FILENO);
        close(p1[0]); close(p1[1]);
        execlp("sh", "sh", "-c", command, (char *)NULL);

        perror("execlp failed");
        exit(EXIT_FAILURE);
    }

    close(p1[1]);

    while(true){
        char buf[MSG_SIZE];
        int bytes_read = 0;

        while(bytes_read < MSG_SIZE - 1){
            ssize_t n = read(p1[0], &buf[bytes_read], 1);
            if(n == -1){
                perror("Error reading from pipe");
                break;
            }

            if(n == 0){
                break;
            }

            bytes_read++;
            if (buf[bytes_read - 1] == '\n'){
                break;
            }
        }
        if (bytes_read == 0) {
            break;
        }

        buf[bytes_read] = '\0';

        if (mq_send(mq, buf, bytes_read + 1, priority) == -1) {
            perror("Error with mq_send");
            break;
        }
    }

    close(mq);
    close(p1[1]);

    return EXIT_SUCCESS;
}
