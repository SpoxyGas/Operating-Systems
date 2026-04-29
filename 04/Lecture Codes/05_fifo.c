#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <linux/stat.h>

#define FIFO_FILE "./my_fifo"

int main(void)
{
        FILE *fp;
        char readbuf[80];

        /* Create the FIFO if it does not exist */
        umask(0);
        mknod(FIFO_FILE, S_IFIFO|0666, 0);

        while(1)
        {
                fp = fopen(FIFO_FILE, "r");
                while (fgets(readbuf, 80, fp) != NULL)
                {
                        printf("Received string: %s\n", readbuf);
                }
                fclose(fp);
                printf ("End of stream reached\n");
        }

        return(0);
}
