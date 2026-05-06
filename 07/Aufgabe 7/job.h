#ifndef JOB_H
#define JOB_H

#define MAX_TIMELINE 10000

typedef struct {
    char name[16];
    int arrival;
    int burst;
    int deadline;

    int remaining;
    int completion;
    int finished;

    int quantum;
    int slice_used;
} Job;

typedef struct {
    char slot[MAX_TIMELINE][16];
    int length;
} Timeline;

void add_to_timeline(Timeline *tl, const char *name);
void print_timeline(const Timeline *tl);
void print_stats(Job jobs[], int n);
int all_done(const Job jobs[], int n);

/* Scheduling functions to implement */
int pick_fifo(const Job jobs[], int n, int now, int last);
int pick_sjf(const Job jobs[], int n, int now, int last);
int pick_rr(const Job jobs[], int n, int now, int last);
int pick_edf(const Job jobs[], int n, int now, int last);

#endif