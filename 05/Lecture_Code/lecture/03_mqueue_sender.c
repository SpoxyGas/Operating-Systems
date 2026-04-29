#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_NAME "/mq_demo_ipc"

int main(void) {
    mqd_t mq = mq_open(QUEUE_NAME, O_WRONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open sender");
        return EXIT_FAILURE;
    }

    if (mq_send(mq, "message with prio 1", strlen("message with prio 1") + 1, 1) == -1) {
        perror("mq_send prio1");
        mq_close(mq);
        return EXIT_FAILURE;
    }

    if (mq_send(mq, "message with prio 9", strlen("message with prio 9") + 1, 9) == -1) {
        perror("mq_send prio9");
        mq_close(mq);
        return EXIT_FAILURE;
    }

    if (mq_send(mq, "message with prio 5", strlen("message with prio 5") + 1, 5) == -1) {
        perror("mq_send prio5");
        mq_close(mq);
        return EXIT_FAILURE;
    }

    if (mq_close(mq) == -1) {
        perror("mq_close sender");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
