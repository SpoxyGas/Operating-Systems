#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHM_NAME "/shm_demo_ipc"

struct shared_data {
    int value;
    char text[64];
};

int main(void) {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if (fd == -1) {
        perror("shm_open writer");
        return EXIT_FAILURE;
    }

    if (ftruncate(fd, sizeof(struct shared_data)) == -1) {
        perror("ftruncate");
        close(fd);
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    struct shared_data *data = mmap(NULL, sizeof(*data),
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap writer");
        close(fd);
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    data->value = 42;
    snprintf(data->text, sizeof(data->text), "hello via shared memory");

    printf("writer: wrote value = %d, text = %s\n",
           data->value, data->text);

    munmap(data, sizeof(*data));
    close(fd);
    return EXIT_SUCCESS;
}
