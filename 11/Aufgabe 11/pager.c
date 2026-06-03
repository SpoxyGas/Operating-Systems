#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PROCESSES 128
#define MAX_ID_LEN 32

typedef struct {
    int frame;
} PageTableEntry;

typedef struct {
    bool active;
    char pid[MAX_ID_LEN];
    int size;
    int pages;
    int fragmentation;
    int created_order;
    PageTableEntry *page_table;
} Process;

/* Global simulator state */
static int memory_size;
static int page_size;
static int frame_count;

static bool *frame_used;
static Process processes[MAX_PROCESSES];
static int creation_counter = 0;

/* ---------- Helper functions already provided ---------- */

static void print_prompt(void) {
    printf("pager>\n");
}

static int ceil_div(int a, int b) {
    return (a + b - 1) / b;
}

static Process *find_process(const char *pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].active && strcmp(processes[i].pid, pid) == 0) {
            return &processes[i];
        }
    }
    return NULL;
}

static Process *find_free_process_slot(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (!processes[i].active) {
            return &processes[i];
        }
    }
    return NULL;
}

static int count_free_frames(void) {
    int count = 0;

    for (int i = 0; i < frame_count; i++) {
        if (!frame_used[i]) {
            count++;
        }
    }

    return count;
}

static int find_lowest_free_frame(void) {
    for (int i = 0; i < frame_count; i++) {
        if (!frame_used[i]) {
            return i;
        }
    }

    return -1;
}

/* ---------- TODO functions for students ---------- */

static void request_memory(const char *pid, int size) {
    /*
     * TODO:
     * 1. Check whether process already exists.
     * 2. Compute number of pages.
     * 3. Check whether enough free frames exist.
     * 4. Create process entry.
     * 5. Allocate page table.
     * 6. Assign lowest available frames.
     * 7. Print required success/error message.
     */
}

static void release_memory(const char *pid) {
    /*
     * TODO:
     * 1. Find process.
     * 2. If not found, print required error.
     * 3. Free all frames used by this process.
     * 4. Free page table.
     * 5. Mark process inactive.
     * 6. Print required success message.
     */
}

static void translate_address(const char *pid, int logical_address) {
    /*
     * TODO:
     * 1. Find process.
     * 2. Check invalid process.
     * 3. Check invalid logical address.
     * 4. Compute page number and offset.
     * 5. Use page table to find frame.
     * 6. Compute physical address.
     * 7. Print required message.
     */
}

static int compare_processes_by_creation_order(const void *a, const void *b) {
    const Process *pa = *(const Process * const *)a;
    const Process *pb = *(const Process * const *)b;

    return pa->created_order - pb->created_order;
}

static void print_status(void) {
    /*
     * TODO:
     * 1. Collect active processes.
     * 2. Sort by creation order.
     * 3. Print each process and its page table.
     * 4. Print free frames in increasing order.
     */
}

static void cleanup(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].active) {
            free(processes[i].page_table);
            processes[i].page_table = NULL;
            processes[i].active = false;
        }
    }

    free(frame_used);
}

/* ---------- Main command parser already provided ---------- */

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./pager <memory_size> <page_size>\n");
        return EXIT_FAILURE;
    }

    memory_size = atoi(argv[1]);
    page_size = atoi(argv[2]);
    frame_count = memory_size / page_size;

    frame_used = calloc((size_t)frame_count, sizeof(bool));
    if (frame_used == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        return EXIT_FAILURE;
    }

    char line[256];

    print_prompt();

    while (fgets(line, sizeof(line), stdin) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        char command[32];

        if (sscanf(line, "%31s", command) != 1) {
            print_prompt();
            continue;
        }

        if (strcmp(command, "X") == 0) {
            break;
        }

        if (strcmp(command, "RQ") == 0) {
            char pid[MAX_ID_LEN];
            int size;
            sscanf(line, "%*s %31s %d", pid, &size);
            request_memory(pid, size);

        } else if (strcmp(command, "RL") == 0) {
            char pid[MAX_ID_LEN];
            sscanf(line, "%*s %31s", pid);
            release_memory(pid);

        } else if (strcmp(command, "ADDR") == 0) {
            char pid[MAX_ID_LEN];
            int logical_address;
            sscanf(line, "%*s %31s %d", pid, &logical_address);
            translate_address(pid, logical_address);

        } else if (strcmp(command, "STAT") == 0) {
            print_status();
        }

        print_prompt();
    }

    cleanup();
    return EXIT_SUCCESS;
}