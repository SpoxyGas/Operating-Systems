#define _GNU_SOURCE
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/mman.h> 
#include <sys/wait.h> 
#define PAGE_SIZE 4096 

int main(void) { 
	int *shared = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
	
	if (shared == MAP_FAILED) {
	 	perror("mmap"); 
	 	return 1; 
	 	} 
	 	
	shared[0] = 0; 
	printf("Parent PID: %d\n", getpid()); 
	printf("Shared mapping address: %p\n", (void *)shared); 
	printf("Press Enter to fork...\n"); 
	getchar(); 
	pid_t pid = fork(); 
	
	if (pid < 0) { 
		perror("fork"); 
		return 1; 
		} 
		
	if (pid == 0) { 
		printf("\n[Child]\n"); 
		printf("PID: %d\n", getpid()); 
		printf("Shared address: %p\n", (void *)shared); 
		sleep(2); 
		printf("Value observed by child: %d\n", shared[0]); 
		printf("Press Enter to exit child process...\n"); 
		getchar();
		munmap(shared, PAGE_SIZE); 
		return 0; } 
	else { 
		printf("\n[Parent]\n"); 
		printf("PID: %d\n", getpid()); 
		printf("Child PID: %d\n", pid); 
		shared[0] = 123; 
		printf("Parent wrote shared[0] = 123\n"); 
		wait(NULL); 
		munmap(shared, PAGE_SIZE); 
		return 0; 
		} 
}
