#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
	printf("Start (pid=%d)\n", getpid());
	
	pid_t pid = fork();
	if (pid == 0) {
		// Child
		printf("Child (pid=%d): exiting\n", getpid());
		exit(0);
	} else {
		// Parent
		wait(NULL);
		printf("Parent (pid=%d): child finished\n", getpid());
	}
	printf("End (pid=%d)\n", getpid());
	return 0;
}
