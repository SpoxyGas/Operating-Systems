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
    fprintf(stderr, "Usage: %s <msg-queue-name> <timeout> <command>\n", program_name);
}

static bool parse_timeout(const char *text, unsigned int *value)
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

static bool install_sigpipe_handler(void)
{
    struct sigaction action = {0};

    action.sa_handler = SIG_IGN;
    action.sa_flags = 0;
    if (sigemptyset(&action.sa_mask) == -1) {
        return false;
    }
    return sigaction(SIGPIPE, &action, NULL) != -1;
}

int main(int argc, char *argv[])
{
    const char *queue_name = NULL;
    const char *command = NULL;
    unsigned int timeout_us = 0;

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

    if (!parse_timeout(argv[2], &timeout_us)) {
        fprintf(stderr, "Error: <timeout> must be a non-negative integer\n");
        return EXIT_FAILURE;
    }

    if (!install_sigpipe_handler()) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    mq_unlink(queue_name);

    struct mq_attr attr = {0};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;

    mqd_t mq = mq_open(queue_name, O_CREAT | O_RDONLY, 0600, &attr);

    if (mq == (mqd_t)-1){
        perror("Error opening queue");
        return EXIT_FAILURE;
    }

    int p1[2];
    if (pipe(p1) == -1) {
        perror("Error opening pipe");
        return EXIT_FAILURE;
    }

    pid_t pidChild = fork();
    if (pidChild == -1) {
        perror("Error forking");
        return EXIT_FAILURE;
    }
    
    if(pidChild == 0){
        dup2(p1[0], STDIN_FILENO);
        close(p1[0]); close(p1[1]);
        execlp("sh", "sh", "-c", command, (char *)NULL);

        perror("execlp failed");
        exit(EXIT_FAILURE);
    }

    close(p1[0]);

    while(true){
        char buf[MSG_SIZE];
        unsigned int prio = 0;
        ssize_t n = mq_receive(mq, buf, sizeof(buf), &prio);

        if(n == -1){
            perror("Error with mq_receive");
            break;
        }
        if(write(p1[1], buf, n - 1) == -1){
            if (errno == EPIPE) {
                break;
            }
        }
        usleep(timeout_us);
        
        int status;
        pid_t result = waitpid(pidChild, &status, WNOHANG);
        if (result > 0){
            break;
        } else if (result == -1){
            break;
        }
    }
    close(p1[1]);
    mq_close(mq);
    mq_unlink(queue_name);

    return EXIT_SUCCESS;
}
