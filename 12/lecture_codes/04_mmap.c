#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main() {

    int fd = open("file1.txt", O_RDWR);
    struct stat st;
    stat("file1.txt", &st);

    char *data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (data == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("File content: %s\n", data);
    printf("PID: %d\n", getpid());
    getchar();
    data[0] = 'X';
    munmap(data, st.st_size);
    close(fd);
    return 0;
}
