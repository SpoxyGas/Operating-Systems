#include <stdio.h>
#include <stdlib.h>

int global_var = 10;

int main() {
    int local_var = 20;

    void *heap_ptr = malloc(4096);

    printf("global: %p\n", &global_var);
    printf("local : %p\n", &local_var);
    printf("heap  : %p\n", heap_ptr);

    getchar();
    return 0;
}
