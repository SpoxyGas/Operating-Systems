#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_FILE "/tmp/myfifo"
#define BUFFER_SIZE 80

int main(void) {
    umask(0);

    if (mkfifo(FIFO_FILE, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            return EXIT_FAILURE;
        }
    }

    while(1) {
        char buffer[BUFFER_SIZE];
        
        int n = read(0, buffer, BUFFER_SIZE);
        if (n <= 0) break;
        
        int fd_write = open(FIFO_FILE, O_WRONLY);
        write(fd_write, buffer, n);
        close(fd_write);

        int fd_read = open(FIFO_FILE, O_RDONLY);
        n = read(fd_read, buffer, BUFFER_SIZE);
        close(fd_read);

        if (n > 0) {
            write(1, buffer, n);
        }
    }

    return 0;
}