#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
        close(fd[1]);

        /* Close stdin, duplicate the input side of pipe to stdin */
        dup2(fd[0], STDIN_FILENO); // Note: STDIN_FILENO == 0
        execlp("sort", "sort", NULL);
    }
    else
    {
        /* Parent process closes up input side of pipe */
        close(fd[0]);

        char *s = "Bob\nAlice\nCharlie\n";
        write (fd[1], s, strlen(s));

        close(fd[1]);
        waitpid(childpid, NULL, 0);
    }
}