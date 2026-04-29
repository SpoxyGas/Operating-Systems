#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#define SHM_NAME "/ps_matrix_mult_shm"

struct shared_data {
    int rows_a;
    int cols_a;
    int cols_b;
    int data[];
};

#endif
