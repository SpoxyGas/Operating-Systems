#include <stdlib.h>

int main() {
    const long N = 1024L * 1024L * 1024L;

    char *p = malloc(N);

    for (long i = 0; i < N; i += 4096)
        p[i]++;

    free(p);
}
