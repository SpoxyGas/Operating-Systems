#include <stdio.h>
#include <sys/stat.h>

int main() {

    struct stat st;

    if (stat("../file.txt", &st) == -1) {
        perror("stat");
        return 1;
    }

    printf("Inode Number : %lu\n", st.st_ino);
    printf("File Size    : %ld bytes\n", st.st_size);
    printf("Link Count   : %ld\n", st.st_nlink);

    return 0;
}