#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("../file.txt", O_RDONLY);
    printf("Current offset: %ld\n",
           lseek(fd, 0, SEEK_CUR));
    lseek(fd, 7, SEEK_SET);
    printf("Current offset: %ld\n",
           lseek(fd, 0, SEEK_CUR));
    char buf[8];
    ssize_t n = read(fd, buf, 6);
    if (n == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    buf[n] = '\0';
    printf("Read data: %s\n", buf);
    close(fd);
    return 0;
}