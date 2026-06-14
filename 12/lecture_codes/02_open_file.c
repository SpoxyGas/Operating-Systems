#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("../temp.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    printf("PID: %d\n", getpid());
    getchar();
    char buf[64];
    int n = read(fd, buf, sizeof(buf)-1);
    buf[n] = '\0';
    printf("Content: %s\n", buf);
    close(fd);
    return 0;
}