#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
	pid_t pid = fork();
	if (pid == 0) {
		// Child
		printf("I'm the child %d\n", getpid());
		sleep(1);
		return 42;
	} else {
		// Parent
		int status;
		printf("My child is called %d\n", pid);
		waitpid(pid, &status, 0);
 		// wait for THIS child only
		if (WIFEXITED(status)) {
			printf("My child exited with status %d\n", WEXITSTATUS(status));
		}
	printf("My child has terminated\n");
	}
	printf("This is the end (%d)\n", getpid());
	return 0;
}
