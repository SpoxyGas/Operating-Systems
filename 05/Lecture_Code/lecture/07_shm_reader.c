#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHM_NAME "/shm_demo_ipc"

struct shared_data {
    int value;
    char text[64];
};

int main(void) {
    int fd = shm_open(SHM_NAME, O_RDONLY, 0);
    if (fd == -1) {
        perror("shm_open reader");
        return EXIT_FAILURE;
    }

    struct shared_data *data = mmap(NULL, sizeof(*data),
                                    PROT_READ,
                                    MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap reader");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("reader: read value = %d, text = %s\n",
           data->value, data->text);

    munmap(data, sizeof(*data));
    close(fd);

    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
