#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define QUEUE_NAME "/mq_demo_ipc"
#define MSG_SIZE 128

int main(void)
{
    struct mq_attr attr = {0};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;
    pid_t pid = getpid();

    mq_unlink(QUEUE_NAME);

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0600, &attr);
    if (mq == (mqd_t)-1)
    {
        perror("mq_open receiver");
        return EXIT_FAILURE;
    }

    printf("receiver %d: queue created (%s)\n", pid, QUEUE_NAME);
    printf("receiver %d: waiting for sender...\n", pid);

    for (int i = 0; i < 3; i++)
    {
        sleep(3);
        char buf[MSG_SIZE];
        unsigned int prio = 0;
        ssize_t n = mq_receive(mq, buf, sizeof(buf), &prio);
        if (n == -1)
        {
            perror("mq_receive");
            mq_close(mq);
            mq_unlink(QUEUE_NAME);
            return EXIT_FAILURE;
        }
        printf("receiver %d: got (prio %u): %s\n", pid, prio, buf);
    }

    if (mq_close(mq) == -1)
    {
        perror("mq_close receiver");
        mq_unlink(QUEUE_NAME);
        return EXIT_FAILURE;
    }

    if (mq_unlink(QUEUE_NAME) == -1)
    {
        perror("mq_unlink receiver");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
