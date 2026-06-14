#include <stdio.h>
#include <unistd.h>

int main() {

    FILE *fp = fopen("../log.txt", "a");

    for (int i = 0; i < 10; i++) {
        fprintf(fp, "PID %d: message %d\n", getpid(), i);
        fflush(fp);
        sleep(1);
    }

    fclose(fp);
    return 0;
}