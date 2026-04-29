#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void print_usage(const char *program_name)
{
    fprintf(stderr, "Usage: %s \"command1\" \"command2\" \"command3\" [-d]\n", program_name);
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

int main (int argc, char *argv[]){
    bool debug_mode = false;

    if (argc != 4 && argc != 5) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 5) {
        if (strcmp(argv[4], "-d") != 0) {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
        debug_mode = true;
    }

    if (!install_sigpipe_handler()) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    const char* cmd1 = argv[1];
    const char* cmd2 = argv[2];
    const char* cmd3 = argv[3];

    int p1[2], p2[2], p3[2];
    if (pipe(p1) == -1 || pipe(p2) == -1 || pipe(p3) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }


    pid_t pid1 = fork();
    if (pid1 < 0){
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid1 == 0){
        dup2(p1[1], STDOUT_FILENO);
        close(p1[0]); close(p1[1]); 
        close(p2[0]); close(p2[1]); 
        close(p3[0]); close(p3[1]);
        execlp("sh", "sh", "-c", cmd1, (char *)NULL);
        perror("execlp cmd1");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 < 0){
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid2 == 0){
        dup2(p2[1], STDOUT_FILENO);
        close(p1[0]); close(p1[1]); 
        close(p2[0]); close(p2[1]); 
        close(p3[0]); close(p3[1]);
        execlp("sh", "sh", "-c", cmd2, (char *)NULL);
        perror("execlp cmd2");
        exit(EXIT_FAILURE);
    }

    pid_t pid3 = fork();
    if (pid3 < 0){
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid3 == 0){
        dup2(p3[0], STDIN_FILENO);
        close(p1[0]); close(p1[1]); 
        close(p2[0]); close(p2[1]); 
        close(p3[0]); close(p3[1]);
        execlp("sh", "sh", "-c", cmd3, (char *)NULL);
        perror("execlp cmd3");
        exit(EXIT_FAILURE);
    }
    
    close(p1[1]);
    close(p2[1]);
    close(p3[0]);

    FILE *stream1 = fdopen(p1[0], "r");
    FILE *stream2 = fdopen(p2[0], "r");
    int out_fd = p3[1];

    bool cmd1_active = true;
    bool cmd2_active = true;
    bool consumer_active = true;

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while (cmd1_active || cmd2_active) {
        if (cmd1_active) {
            if (debug_mode == 1){
                printf("waiting for command1 output\n");
                fflush(stdout);
            }
            nread = getline(&line, &len, stream1);

            if (nread == -1) {
                cmd1_active = false;
                if (debug_mode) {
                    printf("command1 output closed\n");
                    fflush(stdout);
                }
            }else if (consumer_active) {
                if (write(out_fd, line, nread) == -1) {
                    consumer_active = false;
                    if (debug_mode) {
                        printf("command3 input closed\n");
                        fflush(stdout);
                    }
                }
            }
        }

        if (cmd2_active) {
            if (debug_mode) {
                printf("waiting for command2 output\n");
                fflush(stdout);
            }
            
            nread = getline(&line, &len, stream2);
            
            if (nread == -1) {
                cmd2_active = false;
                if (debug_mode) {
                    printf("command2 output closed\n");
                    fflush(stdout);
                }
            } else if (consumer_active) {
                if (write(out_fd, line, nread) == -1) {
                    consumer_active = false;
                    if (debug_mode) {
                        printf("command3 input closed\n");
                        fflush(stdout);
                    }
                }
            }
        }
    }

    free(line);
    fclose(stream1);
    fclose(stream2);
    close(out_fd);

    int status;
    int exit_code = EXIT_SUCCESS;

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, &status, 0);

    if (WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
    }

    return exit_code;
}
