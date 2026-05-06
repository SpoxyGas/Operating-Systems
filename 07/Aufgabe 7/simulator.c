#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "job.h"

#define MAX_JOBS 128

typedef enum {
    POLICY_FIFO,
    POLICY_SJF,
    POLICY_RR,
    POLICY_EDF
} Policy;

typedef int (*PickFunction)(const Job jobs[], int n, int now, int last);

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s <fifo|sjf|rr|edf> <workload-file> [quantum]\n", prog);
}

static int parse_policy(const char *text, Policy *policy) {
    if (strcmp(text, "fifo") == 0) {
        *policy = POLICY_FIFO;
        return 1;
    }

    if (strcmp(text, "sjf") == 0) {
        *policy = POLICY_SJF;
        return 1;
    }

    if (strcmp(text, "rr") == 0) {
        *policy = POLICY_RR;
        return 1;
    }

    if (strcmp(text, "edf") == 0) {
        *policy = POLICY_EDF;
        return 1;
    }

    return 0;
}

static int read_workload(const char *path, Job jobs[], int max_jobs, int quantum) {
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        perror(path);
        return -1;
    }

    int n = 0;
    int line_no = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL) {
        line_no++;

        if (line[0] == '\n' || line[0] == '#') {
            continue;
        }

        if (n >= max_jobs) {
            fprintf(stderr, "Too many jobs. Maximum is %d.\n", max_jobs);
            fclose(file);
            return -1;
        }

        int id;
        Job j;

        int fields = sscanf(line, "%d %d %d %d",
                            &id,
                            &j.arrival,
                            &j.burst,
                            &j.deadline);

        if (fields != 4) {
            fprintf(stderr, "Invalid workload line %d: %s", line_no, line);
            fclose(file);
            return -1;
        }

        if (j.arrival < 0 || j.burst <= 0 || j.deadline < 0) {
            fprintf(stderr,
                    "Invalid job on line %d: arrival/deadline must be >= 0 and burst must be > 0.\n",
                    line_no);
            fclose(file);
            return -1;
        }

        snprintf(j.name, sizeof(j.name), "T%d", id);
        j.remaining = j.burst;
        j.completion = -1;
        j.finished = 0;
        j.quantum = quantum;
        j.slice_used = 0;

        jobs[n++] = j;
    }

    fclose(file);
    return n;
}

int all_done(const Job jobs[], int n) {
    for (int i = 0; i < n; i++) {
        if (!jobs[i].finished) {
            return 0;
        }
    }

    return 1;
}

void add_to_timeline(Timeline *tl, const char *name) {
    if (tl->length >= MAX_TIMELINE) {
        fprintf(stderr, "Timeline limit exceeded.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(tl->slot[tl->length], name, sizeof(tl->slot[tl->length]) - 1);
    tl->slot[tl->length][sizeof(tl->slot[tl->length]) - 1] = '\0';
    tl->length++;
}

void print_timeline(const Timeline *tl) {
    for (int t = 0; t < tl->length; t++) {
        printf("[%2d-%2d] %s\n", t, t + 1, tl->slot[t]);
    }
}

void print_stats(Job jobs[], int n) {
    double avg_wait = 0.0;
    double avg_turn = 0.0;
    int missed_count = 0;

    printf("Results:\n");
    printf("%-8s %-8s %-8s %-10s %-12s %-12s %-8s\n",
           "Task","Arrival","Burst","Deadline","Waiting","Turnaround","Missed");

    for (int i = 0; i < n; i++) {
        int turnaround = jobs[i].completion - jobs[i].arrival;
        int waiting = turnaround - jobs[i].burst;
        int missed = jobs[i].completion > jobs[i].deadline;

        avg_wait += waiting;
        avg_turn += turnaround;
        if (missed) missed_count++;

        printf("%-8s %-8d %-8d %-10d %-12d %-12d %-8s\n",
               jobs[i].name,
               jobs[i].arrival,
               jobs[i].burst,
               jobs[i].deadline,
               waiting,
               turnaround,
               missed ? "yes" : "no");
    }

    printf("\nAverage waiting time: %.2f\n", avg_wait / n);
    printf("Average turnaround time: %.2f\n", avg_turn / n);
    printf("Missed deadlines: %d/%d\n", missed_count, n);
}

static void validate_choice(const Job jobs[], int n, int now, int selected) {
    if (selected == -1) {
        return;
    }

    if (selected < 0 || selected >= n) {
        fprintf(stderr, "Scheduler returned invalid index %d at time %d.\n", selected, now);
        exit(EXIT_FAILURE);
    }

    if (jobs[selected].remaining <= 0) {
        fprintf(stderr, "Scheduler selected completed job %s at time %d.\n",
                jobs[selected].name, now);
        exit(EXIT_FAILURE);
    }

    if (jobs[selected].arrival > now) {
        fprintf(stderr, "Scheduler selected job %s before arrival time at time %d.\n",
                jobs[selected].name, now);
        exit(EXIT_FAILURE);
    }
}

static void simulate_policy(Job jobs[], int n, PickFunction pick, const char *title) {
    Timeline tl = { .length = 0 };
    int now = 0;
    int last = -1;

    while (!all_done(jobs, n)) {
        int selected = pick(jobs, n, now, last);

        validate_choice(jobs, n, now, selected);

        if (selected == -1) {
            add_to_timeline(&tl, "IDLE");
            now++;
            last = -1;
            continue;
        }

        if (selected != last) {
            jobs[selected].slice_used = 0;
        }

        add_to_timeline(&tl, jobs[selected].name);

        jobs[selected].remaining--;
        jobs[selected].slice_used++;

        now++;

        if (jobs[selected].remaining == 0) {
            jobs[selected].completion = now;
            jobs[selected].finished = 1;
            jobs[selected].slice_used = 0;
            last = -1;
        } else {
            last = selected;
        }
    }

    printf("\n=== %s ===\n", title);
    print_timeline(&tl);
    print_stats(jobs, n);
}

int main(int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    Policy policy;

    if (!parse_policy(argv[1], &policy)) {
        fprintf(stderr, "Unknown policy: %s\n", argv[1]);
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int quantum = 2;

    if (argc == 4) {
        quantum = atoi(argv[3]);

        if (quantum <= 0) {
            fprintf(stderr, "Quantum must be a positive integer.\n");
            return EXIT_FAILURE;
        }
    }

    if (policy != POLICY_RR && argc == 4) {
        fprintf(stderr, "Warning: quantum argument is ignored for non-RR policies.\n");
    }

    Job jobs[MAX_JOBS];
    int n = read_workload(argv[2], jobs, MAX_JOBS, quantum);

    if (n <= 0) {
        fprintf(stderr, "No jobs loaded.\n");
        return EXIT_FAILURE;
    }

    switch (policy) {
    case POLICY_FIFO:
        simulate_policy(jobs, n, pick_fifo, "FIFO Scheduling");
        break;

    case POLICY_SJF:
        simulate_policy(jobs, n, pick_sjf, "SJF Scheduling");
        break;

    case POLICY_RR:
        simulate_policy(jobs, n, pick_rr, "Round Robin Scheduling");
        break;

    case POLICY_EDF:
        simulate_policy(jobs, n, pick_edf, "EDF Scheduling");
        break;
    }

    return EXIT_SUCCESS;
}