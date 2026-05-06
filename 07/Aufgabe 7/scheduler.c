#include "job.h"

/*
 * Return:
 *  - index i (0 <= i < n) of job to run
 *  - or -1 if CPU should be idle
 *
 * Do NOT modify jobs[]. Only inspect and return index.
 */



int pick_fifo(const Job jobs[], int n, int now, int last) {

    if (last != -1 && jobs[last].remaining > 0) {
        return last;
    }

    int best_index = -1;

    for (int i = 0; i < n; i++) {
        if (jobs[i].arrival <= now && jobs[i].remaining > 0) {
            if (best_index == -1) {
                best_index = i;
            } else if (jobs[i].arrival < jobs[best_index].arrival) {
                best_index = i;
            }
        }
    }
    return best_index;
}



int pick_sjf(const Job jobs[], int n, int now, int last) {

    if (last != -1 && jobs[last].remaining > 0) {
        return last;
    }

    int best_index = -1;

    for (int i = 0; i < n; i++){
        if (jobs[i].arrival <= now && jobs[i].remaining > 0) {
            if (best_index == -1) {
                best_index = i;
            } else if (jobs[i].burst < jobs[best_index].burst) {
                best_index = i;
            } else if (jobs[i].burst == jobs[best_index].burst) {
                if (jobs[i].arrival < jobs[best_index].arrival) {
                    best_index = i;
                }
            }
        }
    }
    return best_index;
}



int pick_rr(const Job jobs[], int n, int now, int last) {
    
    if (last != -1 && jobs[last].remaining > 0 && jobs[last].slice_used < jobs[last].quantum) {
        return last;
    }

    int starting_index = (last == -1) ? 0 : (last + 1) % n;

    for (int i = 0; i < n; i++){
        int check_index = (starting_index + i) % n;

        if (jobs[check_index].arrival <= now && jobs[check_index].remaining > 0) {
            return check_index;
        }
    }
    return -1;
}



int pick_edf(const Job jobs[], int n, int now, int last) {
    
    (void)last;

    int best_index = -1;

    for (int i = 0; i < n; i++) {
        if (jobs[i].arrival <= now && jobs[i].remaining > 0) {
            if (best_index == -1) {
                best_index = i;
            } else if (jobs[i].deadline < jobs[best_index].deadline) {
                best_index = i;
            } else if (jobs[i].deadline == jobs[best_index].deadline) {
                if (jobs[i].arrival < jobs[best_index].arrival) {
                    best_index = i;
                }
            }
        }
    }
    return best_index;
}
