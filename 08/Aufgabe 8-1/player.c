#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t got_attack = 0;
volatile sig_atomic_t got_shield = 0;
volatile sig_atomic_t got_alarm  = 0;
volatile sig_atomic_t got_quit   = 0;

void handler_usr1(int signo) { 
    (void) signo;
    got_attack = 1;
}
void handler_usr2(int signo) { 
    (void) signo;
    got_shield = 1; 
}
void handler_alrm(int signo) { 
    (void) signo;
    got_alarm = 1; 
}
void handler_int(int signo)  {     
    (void) signo;
    got_quit = 1;
}

typedef enum {NORMAL, SHIELDED, FROZEN} State;

int main (int argc, char* argv[]) {

    State current_state = NORMAL;

    printf("[PLAYER %d] Commands: s = shield, f = freeze, q = quit\n", getpid());
    printf("[PLAYER] State=NORMAL Health=5 Score=0\n");

    if (argc > 1 && strcmp(argv[1], "--auto-freeze") == 0) {
        current_state = FROZEN;
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);
        sigprocmask(SIG_BLOCK, &set, NULL);
        alarm(5);
        printf("[PLAYER] Frozen for 5 seconds; SIGUSR1 blocked\n");
    }

    struct sigaction act;

    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0; 
    
    act.sa_handler = handler_usr1;
    sigaction(SIGUSR1, &act, NULL);
    
    act.sa_handler = handler_usr2;
    sigaction(SIGUSR2, &act, NULL);
    
    act.sa_handler = handler_alrm;
    sigaction(SIGALRM, &act, NULL);
    
    act.sa_handler = handler_int;
    sigaction(SIGINT, &act, NULL);

    int health = 5;
    int score = 0;

    char cmd;
    while (1) {
        ssize_t n = read(STDIN_FILENO, &cmd, 1);

        if (n < 0) {
            if (errno != EINTR) {
                break;
            }
        } else if (n > 0) {
            if (cmd == 'q') {
                printf("[PLAYER] Quit\n");
                exit(0);
            } else if (cmd == 's') {
                current_state = SHIELDED;
                alarm(3);
                printf("[PLAYER] Shield activated for 3 seconds\n");
            } else if (cmd == 'f') {
                current_state = FROZEN;
                sigset_t set;
                sigemptyset(&set);
                sigaddset(&set, SIGUSR1);
                sigprocmask(SIG_BLOCK, &set, NULL);
                alarm(5);
                printf("[PLAYER] Frozen for 5 seconds; SIGUSR1 blocked\n");
            }
        }

        if (got_quit) {
            got_quit = 0;
            printf("[PLAYER] Quit\n");
            exit(0);
        }

        if (got_alarm) {
            got_alarm = 0;
            if (current_state == SHIELDED) {
                current_state = NORMAL;
                printf("[PLAYER] Shield expired; back to NORMAL\n");
            } else if (current_state == FROZEN) {
                printf("[PLAYER] Freeze expired; unblocking SIGUSR1\n");
                sigset_t set;
                sigemptyset(&set);
                sigaddset(&set, SIGUSR1);
                sigprocmask(SIG_UNBLOCK, &set, NULL); 
                current_state = NORMAL;
                printf("[PLAYER] Back to NORMAL\n");
            }
        }

        if (got_attack) {
            got_attack = 0;
            if (current_state == SHIELDED) {
                score++;
                printf("[PLAYER] Attack ignored by shield. Score=%d\n", score);
            } else if (current_state == NORMAL) { 
                health--;
                printf("[PLAYER] Attack delivered. Health=%d\n", health);
                if (health <= 0) {
                    printf("[PLAYER] Game over\n");
                    exit(0);
                }
            }
        }

        if (got_shield) {
            got_shield = 0;
            current_state = SHIELDED;
            alarm(3);
            printf("[PLAYER] Shield activated by SIGUSR2 for 3 seconds\n");
        }
    }
    return 0;
}