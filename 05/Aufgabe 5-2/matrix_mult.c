#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shared_data.h"

struct matrix
{
    int rows;
    int cols;
    int *values;
};

static void print_usage(const char *program_name)
{
    fprintf(stderr, "Usage: %s <matrix-a-file> <matrix-b-file>\n", program_name);
}

static void free_matrix(struct matrix *matrix)
{
    free(matrix->values);
    matrix->values = NULL;
    matrix->rows = 0;
    matrix->cols = 0;
}

static bool read_matrix(const char *path, struct matrix *matrix)
{
    FILE *file = NULL;
    size_t cell_count = 0;

    file = fopen(path, "r");
    if (file == NULL)
    {
        perror(path);
        return false;
    }

    if (fscanf(file, "%d %d", &matrix->rows, &matrix->cols) != 2 || matrix->rows <= 0 || matrix->cols <= 0)
    {
        fprintf(stderr, "Error: invalid matrix file '%s'\n", path);
        fclose(file);
        return false;
    }

    cell_count = (size_t)matrix->rows * (size_t)matrix->cols;
    matrix->values = calloc(cell_count, sizeof(int));
    if (matrix->values == NULL)
    {
        perror("calloc");
        fclose(file);
        return false;
    }

    for (size_t index = 0; index < cell_count; ++index)
    {
        if (fscanf(file, "%d", &matrix->values[index]) != 1)
        {
            fprintf(stderr, "Error: invalid matrix file '%s'\n", path);
            fclose(file);
            free_matrix(matrix);
            return false;
        }
    }

    fclose(file);
    return true;
}

int main(int argc, char *argv[])
{
    struct matrix matrix_a = {0};
    struct matrix matrix_b = {0};

    if (argc != 3)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (!read_matrix(argv[1], &matrix_a) || !read_matrix(argv[2], &matrix_b))
    {
        free_matrix(&matrix_b);
        free_matrix(&matrix_a);
        return EXIT_FAILURE;
    }

    if (matrix_a.cols != matrix_b.rows)
    {
        fprintf(stderr, "Error: matrices cannot be multiplied\n");
        free_matrix(&matrix_b);
        free_matrix(&matrix_a);
        return EXIT_FAILURE;
    }

    
    size_t data_size_a = matrix_a.rows * matrix_a.cols;
    size_t data_size_b = matrix_b.rows * matrix_b.cols;
    size_t data_size_c = matrix_a.rows * matrix_b.cols;
    size_t total_shm_size = sizeof(struct shared_data) + (data_size_a + data_size_b + data_size_c) * sizeof(int);

    shm_unlink(SHM_NAME);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if (shm_fd == -1) {
        perror("Error opening shared memory");
        free_matrix(&matrix_b);
        free_matrix(&matrix_a);
        return EXIT_FAILURE;
    }

    if (ftruncate(shm_fd, total_shm_size) == -1) {
        perror("Error ftruncate");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        free_matrix(&matrix_b);
        free_matrix(&matrix_a);
        return EXIT_FAILURE;
    }

    struct shared_data *shm_ptr = mmap(NULL, total_shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("Error mmap");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        free_matrix(&matrix_b);
        free_matrix(&matrix_a);
        return EXIT_FAILURE;
    }

    shm_ptr->rows_a = matrix_a.rows;
    shm_ptr->cols_a = matrix_a.cols;
    shm_ptr->cols_b = matrix_b.cols;

    size_t offset_a = 0;
    size_t offset_b = data_size_a;
    size_t offset_c = data_size_a + data_size_b;

    for (size_t i = 0; i < data_size_a; i++) {
        shm_ptr->data[offset_a + i] = matrix_a.values[i];
    }
    for (size_t i = 0; i < data_size_b; i++) {
        shm_ptr->data[offset_b + i] = matrix_b.values[i];
    }
    for (size_t i = 0; i < data_size_c; i++) {
        shm_ptr->data[offset_c + i] = 0; 
    }

    
    for (int r = 0; r < matrix_a.rows; r++) {
        for (int c = 0; c < matrix_b.cols; c++) {
            pid_t pid = fork();
            
            if (pid == -1) {
                perror("Error forking");
                exit(EXIT_FAILURE); 
            }
            
            if (pid == 0) {
                char row_str[16];
                char col_str[16];
                snprintf(row_str, sizeof(row_str), "%d", r);
                snprintf(col_str, sizeof(col_str), "%d", c);
                
                execlp("./matrix_worker", "matrix_worker", row_str, col_str, (char *)NULL);
                
                perror("Error execlp");
                exit(EXIT_FAILURE);
            }
        }
    }
    
    int num_workers = matrix_a.rows * matrix_b.cols;
    for (int i = 0; i < num_workers; i++) {
        wait(NULL);
    }

    printf("%d %d\n", matrix_a.rows, matrix_b.cols);
    for (int r = 0; r < matrix_a.rows; r++) {
        for (int c = 0; c < matrix_b.cols; c++) {

            int idx = r * matrix_b.cols + c; 
            printf("%d", shm_ptr->data[offset_c + idx]);
            
            if (c < matrix_b.cols - 1) {
                printf(" ");
            }
        }
        printf("\n");
    }

    
    munmap(shm_ptr, total_shm_size);
    close(shm_fd);
    shm_unlink(SHM_NAME);

    free_matrix(&matrix_b);
    free_matrix(&matrix_a);
    
    return EXIT_SUCCESS;
}