#include <stdio.h>
#include <time.h>

int main() {
    volatile unsigned long long x = 0;
    time_t start = time(NULL);

    while (time(NULL) - start < 50) {
        x++;
    }

    printf("Done: %llu\n", x);
    return 0;
}