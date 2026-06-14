#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

int main() {

    int fd = open("../log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);

    printf("PID %d waiting for lock...\n", getpid());

    flock(fd, LOCK_EX);

    printf("PID %d acquired lock\n",
           getpid());

    for (int i = 0; i < 5; i++) {
        printf("PID %d: message %d\n", getpid(), i);
        dprintf(fd,"PID %d: message %d\n", getpid(), i);
        sleep(1);
    }

    flock(fd, LOCK_UN);
    printf("PID %d released lock\n", getpid());
    close(fd);
    return 0;
}