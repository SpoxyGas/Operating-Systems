#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pid = fork();
    if(pid == 0) {
        int child = getpid();
        printf("child: parent %d, group %d\n", getppid(), getpgid(child));
        sleep(20);
        printf("child: parent %d, group %d\n", getppid(), getpgid(child));
        sleep(20);
        printf("child: parent %d, group %d\n", getppid(), getpgid(child));
    } else {
        int parent = getpid();
        printf("parent: parent %d, group %d\n", getppid(), getpgid(parent));
        sleep(20);
    }
    return 0;
}
