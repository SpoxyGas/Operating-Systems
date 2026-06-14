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
    if (find_process(pid) != NULL) {
        printf("Error: process already exists\n");
        return;
    }

    int pages = ceil_div(size, page_size);

    if (pages > count_free_frames()) {
        printf("Error: insufficient memory\n");
        return;
    }

    Process *p = find_free_process_slot();
    if (p == NULL) {
        printf("Error: insufficient memory\n");
        return;
    }

    p->active = true;
    strcpy(p->pid, pid);
    p->size = size;
    p->pages = pages;
    p->fragmentation = (pages * page_size) - size;
    p->created_order = creation_counter++;

    if (pages > 0) {
        p->page_table = calloc((size_t)pages, sizeof(PageTableEntry));
    } else {
        p->page_table = NULL;
    }

    for (int i = 0; i < pages; i++) {
        int frame = find_lowest_free_frame();
        frame_used[frame] = true;
        p->page_table[i].frame = frame;
    }

    printf("Allocated %d pages to process %s\n", pages, pid);
}

static void release_memory(const char *pid) {
    Process *p = find_process(pid);

    if (p == NULL) {
        printf("Error: process not found\n");
        return;
    }

    for (int i = 0; i < p->pages; i++) {
        frame_used[p->page_table[i].frame] = false;
    }

    free(p->page_table);
    p->page_table = NULL;

    p->active = false;

    printf("Released process %s\n", pid);
}

static void translate_address(const char *pid, int logical_address) {
    Process *p = find_process(pid);

    if (p == NULL) {
        printf("Error: process not found\n");
        return;
    }

    if (logical_address < 0 || logical_address >= p->size) {
        printf("Error: invalid logical address\n");
        return;
    }

    int page_num = logical_address / page_size;
    int offset = logical_address % page_size;

    int frame = p->page_table[page_num].frame;

    int physical_address = (frame * page_size) + offset;

    printf("Logical address %d -> Physical address %d\n", logical_address, physical_address);
}
static int compare_processes_by_creation_order(const void *a, const void *b) {
    const Process *pa = *(const Process * const *)a;
    const Process *pb = *(const Process * const *)b;

    return pa->created_order - pb->created_order;
}

static void print_status(void) {
    const Process *active_procs[MAX_PROCESSES];
    int count = 0;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].active) {
            active_procs[count++] = &processes[i];
        }
    }

    qsort(active_procs, count, sizeof(Process *), compare_processes_by_creation_order);

    for (int i = 0; i < count; i++) {
        const Process *p = active_procs[i];
        printf("Process %s\n", p->pid);
        printf("  Size: %d bytes\n", p->size);
        printf("  Pages: %d\n", p->pages);
        printf("  Internal fragmentation: %d bytes\n", p->fragmentation);
        printf("  Page Table:\n");
        for (int j = 0; j < p->pages; j++) {
            printf("    Page %d -> Frame %d\n", j, p->page_table[j].frame);
        }
    }

    printf("Free Frames:\n");
    if (count_free_frames() == 0) {
        printf("  none\n");
    } else {
        printf(" ");
        for (int i = 0; i < frame_count; i++) {
            if (!frame_used[i]) {
                printf(" %d", i);
            }
        }
        printf("\n");
    }
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