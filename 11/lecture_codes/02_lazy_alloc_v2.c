#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 

int main() {
 	size_t size = 1024L * 1024L * 1024L; 
 	
 	char *p = malloc(size); 
 	
 	printf("Allocated 1 GB\n"); 
 	getchar(); 
 	
 	for (size_t i = 0; i < size; i += 8192) { 
 	p[i] = 1; 
 	} 
 	
 	printf("Touched all pages\n"); 
 	getchar(); 
 	
 	free(p); 
 	}
