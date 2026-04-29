#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "shared_data.h"

static void print_usage(const char *program_name)
{
    fprintf(stderr, "Usage: %s <row> <col>\n", program_name);
}

static bool parse_index(const char *text, int *value)
{
    char *endptr = NULL;
    long parsed = 0;

    if (text[0] == '-')
    {
        return false;
    }

    errno = 0;
    parsed = strtol(text, &endptr, 10);
    if (errno != 0 || endptr == text || *endptr != '\0' || parsed > INT_MAX)
    {
        return false;
    }

    *value = (int)parsed;
    return true;
}

int main(int argc, char *argv[])
{
    int row = 0;
    int col = 0;

    if (argc != 3)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (!parse_index(argv[1], &row) || !parse_index(argv[2], &col))
    {
        fprintf(stderr, "Error: <row> and <col> must be non-negative integers\n");
        return EXIT_FAILURE;
    }

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (shm_fd == -1) {
        perror("Error shm_open in worker");
        return EXIT_FAILURE;
    }

    struct stat sb;
    if (fstat(shm_fd, &sb) == -1) {
        perror("Error fstat in worker");
        close(shm_fd);
        return EXIT_FAILURE;
    }

    struct shared_data *shm_ptr = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("Error mmap in worker");
        close(shm_fd);
        return EXIT_FAILURE;
    }

    int rows_a = shm_ptr->rows_a;
    int cols_a = shm_ptr->cols_a;
    int cols_b = shm_ptr->cols_b;

    size_t offset_a = 0;
    size_t offset_b = rows_a * cols_a;
    size_t offset_c = offset_b + (cols_a * cols_b);

    int sum = 0;
    for (int k = 0; k < cols_a; k++) {
        int val_a = shm_ptr->data[offset_a + (row * cols_a) + k];
        
        int val_b = shm_ptr->data[offset_b + (k * cols_b) + col];
        
        sum += val_a * val_b;
    }

    shm_ptr->data[offset_c + (row * cols_b) + col] = sum;

    munmap(shm_ptr, sb.st_size);
    close(shm_fd);

    return EXIT_SUCCESS;
}