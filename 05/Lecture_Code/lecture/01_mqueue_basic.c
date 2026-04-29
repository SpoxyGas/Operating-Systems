#include <errno.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_NAME "/mq_demo_basic"
#define MSG_SIZE 128

int main(void) {
    struct mq_attr attr = {0};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;

    mq_unlink(QUEUE_NAME);

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0600, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return EXIT_FAILURE;
    }

    if (mq_send(mq, "low priority", strlen("low priority") + 1, 1) == -1) {
        perror("mq_send low");
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        return EXIT_FAILURE;
    }

    if (mq_send(mq, "high priority", strlen("high priority") + 1, 9) == -1) {
        perror("mq_send high");
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        return EXIT_FAILURE;
    }

    if (mq_send(mq, "medium priority", strlen("medium priority") + 1, 5) == -1) {
        perror("mq_send medium");
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 3; i++) {
        char buf[MSG_SIZE];
        unsigned int prio = 0;
        ssize_t n = mq_receive(mq, buf, sizeof(buf), &prio);
        if (n == -1) {
            perror("mq_receive");
            mq_close(mq);
            mq_unlink(QUEUE_NAME);
            return EXIT_FAILURE;
        }
        printf("received (prio %u): %s\n", prio, buf);
    }

    if (mq_close(mq) == -1) {
        perror("mq_close");
        mq_unlink(QUEUE_NAME);
        return EXIT_FAILURE;
    }

    if (mq_unlink(QUEUE_NAME) == -1) {
        perror("mq_unlink");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
