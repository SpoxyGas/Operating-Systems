// Do magic ╰( ͡° ͜ʖ ͡° )つ──☆*:・ﾟ and implement your "Hello world" programm here.

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <time.h>

volatile sig_atomic_t got_sigchild = 0;
volatile sig_atomic_t got_sigalarm = 0;

void handler_sigchld(int sig) {
    (void)sig;
    got_sigchild = 1;
}

void handler_sigalrm(int sig) {
    (void)sig;
    got_sigalarm = 1;
}

static int read_env_int(const char *name, int fallback) {
    const char *value = getenv(name);
    if (value == NULL || value[0] == '\0') {
        return fallback;
    }
    return atoi(value);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <norestart|restart>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int restart_mode = 0;
    if (strcmp(argv[1], "restart") == 0) {
        restart_mode = 1;
        printf("MODE RESTART\n");
    } else if (strcmp(argv[1], "norestart") == 0) {
        restart_mode = 0;
        printf("MODE NO_RESTART\n");
    } else {
        fprintf(stderr, "Invalid mode\n");
        return EXIT_FAILURE;
    }

    srand(time(NULL) ^ getpid());

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    
    sa.sa_flags = restart_mode ? SA_RESTART : 0;

    sa.sa_handler = handler_sigchld;
    sigaction(SIGCHLD, &sa, NULL);

    sa.sa_handler = handler_sigalrm;
    sigaction(SIGALRM, &sa, NULL);

    pid_t child_pid = -1;

    while (1) {

        char buf[16];
        ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
        
        int saved_errno = errno;

        if (n < 0) {
            if (errno == EINTR) {
                printf("READ_EINTR\n");
            } else {
                errno = saved_errno;
                fprintf(stderr, "read error");
                break;
            }
        } else if (n == 0) {
            break;
        }

        if (got_sigalarm) {
            got_sigalarm = 0;
            if (child_pid > 0) {
                printf("TIMEOUT pid=%d\n", child_pid);
                kill(child_pid, SIGKILL); 
            }
        }

        if (got_sigchild) {
            got_sigchild = 0;
            int status;
            pid_t p;
            
            while ((p = waitpid(-1, &status, WNOHANG)) > 0) {
                if (p == child_pid) {
                    alarm(0); 

                    if (WIFEXITED(status)) {
                        printf("TASK_EXIT pid=%d code=%d\n", p, WEXITSTATUS(status));
                    } else if (WIFSIGNALED(status)) {
                        printf("TASK_KILLED pid=%d signal=%d\n", p, WTERMSIG(status));
                    }
                    child_pid = -1;
                }
            }
        }

        if (n < 0) {
            continue;
        }

        if (buf[0] == 'q') {
            if (child_pid > 0) {
                kill(child_pid, SIGKILL);
                waitpid(child_pid, NULL, 0); 
            }
            printf("QUIT\n");
            break;
        } 
        else if (buf[0] == 'r') {
            if (child_pid > 0) continue; 

            int sleep_sec = read_env_int("TASK_SLEEP_SECONDS", (rand() % 5) + 1);
            int exit_code = read_env_int("TASK_EXIT_CODE", rand() % 256);

            child_pid = fork();
            if (child_pid < 0) {
                fprintf(stderr, "fork error");
            } 
            else if (child_pid == 0) {
                sleep(sleep_sec);
                _exit(exit_code);
            } 
            else {
                printf("TASK_STARTED pid=%d\n", child_pid);
                alarm(3); 
            }
        }
    }
    return 0;
}