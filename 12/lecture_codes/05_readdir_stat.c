#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main() {

    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char path[1024];

    dir = opendir("../testdir");

    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {

        int len = snprintf(path, sizeof(path), "../testdir/%s", entry->d_name);

        if (len < 0 || len >= (int)sizeof(path)) {
            fprintf(stderr, "Path too long: %s\n", entry->d_name);
            continue;
        }

        if (stat(path, &st) == 0) {
            printf("Name : %s\n", entry->d_name);
            printf("Inode: %lu\n", (unsigned long) st.st_ino);
            printf("Size : %ld bytes\n\n", st.st_size);
        }
    }

    closedir(dir);

    return 0;
}