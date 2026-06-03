#include <stdio.h>
#include <stdlib.h>

#define N 10000000
#define ITER 50

int main() {
    int *array = malloc((size_t)N * sizeof(int));
    if (array == NULL) {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        array[i] = i % 100;
    }

    volatile long long sum = 0;

    for (int iter = 0; iter < 1000; iter++)
        for (int i = 0; i < 10000; i++)
            sum += array[i];

    free(array);
    return 0;
}
