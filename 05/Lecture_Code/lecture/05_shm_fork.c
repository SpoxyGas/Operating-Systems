#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int *counter = mmap(NULL, sizeof(*counter),
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS,
                        -1, 0);
    if (counter == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    *counter = 0;

    pid_t childpid = fork();
    if (childpid == -1) {
        perror("fork");
        munmap(counter, sizeof(*counter));
        return EXIT_FAILURE;
    }

    if (childpid == 0) {
        for (int i = 0; i < 5; i++) {
            (*counter)++;
        }

        printf("child: counter = %d\n", *counter);
        munmap(counter, sizeof(*counter));
        return EXIT_SUCCESS;
    }

    waitpid(childpid, NULL, 0);
    printf("parent: counter = %d\n", *counter);

    munmap(counter, sizeof(*counter));
    return EXIT_SUCCESS;
}
