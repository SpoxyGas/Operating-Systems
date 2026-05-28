#define _POSIX_C_SOURCE 200809L
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void hidden_checkpoint(const char *tag) {
    printf("%s\n",tag);
    //fflush(stdout);
}
