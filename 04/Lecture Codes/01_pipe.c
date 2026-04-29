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

    /* Let in = fd[0] and out = fd[1].
       Then at the point in code, the data streams through the pipe as follows:

        +---------+      in      +--------+
        |         | <----------- |        |
        | Process |              | Kernel |
        |         | -----------> |        |
        +---------+      out     +--------+

    */

    if((childpid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    /* Now since we have both a parent and child process, that use
       the same file descriptors, the data flow looks like:

        +----------+            +--------+            +---------+
        |  Parent  |     in     |        |     in     |  Child  |
        |  Process | <--------- | Kernel | ---------> | Process |
        |          | ---------> |        | <--------- |         |
        +----------+    out     +--------+    out     +---------+

       In the following code, both the parent and child will close
       one of the pipe's file descriptors after which the data flow
       looks like:

        +----------+            +--------+            +---------+
        |  Parent  |     in     |        |            |  Child  |
        |  Process | <--------- | Kernel | <--------- | Process |
        |          |            |        |    out     |         |
        +----------+            +--------+            +---------+
    */

    if(childpid == 0)
    {
        /* Child process closes up input side of pipe */
        close(fd[0]);

        char *s = "hello from child\n";
        write (fd[1], s, strlen(s));
        close(fd[1]);
        sleep(100);
    }
    else
    {
        /* Parent process closes up output side of pipe */
        close(fd[1]);

        char c;
        while (read(fd[0], &c, 1) > 0)
        {
            putchar(c);
        }
    }
}
