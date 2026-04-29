#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main()
{
    int     fd[2];
    pid_t   childpid;

    pipe(fd);

    if((childpid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if(childpid == 0)
    {
        /* Child process closes up input side of pipe */
        close(fd[0]);

        char *s = "hello from child\n";
        write (fd[1], s, strlen(s));
    }
    else
    {
        /* Parent process closes up output side of pipe */
        //close(fd[1]);

        char c;
        while (read(fd[0], &c, 1) > 0)
        {
            putchar(c);
        }
    }
}