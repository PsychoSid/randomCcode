/* Testing memory allocation and de-allocation */

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>


int main (int argc, char **argv) {
	void *a, *b;

	printf("Process id is %d\n", getpid());

	/* Allocate 500MB */
	printf("Allocating 500MB...\n");
	a = (void *) malloc(500*1024*1024);
	if (a == NULL) {
		fprintf(stderr, "Failed to allocate chunk of memory\n");
	} else {
		printf("500MB allocated\n");
	}
	printf("Sleeping for 10s\n");
	sleep(10);

	/* Deallocate 500MB */
	printf("De-allocating 500MB\n");
	free(a);
	printf("Sleeping for 10s\n");
	sleep(10);


	/* Allocate 500MB */
	printf("Allocating 500MB...\n");
	a = (void *) malloc(500*1024*1024);
	if (a == NULL) {
		fprintf(stderr, "Failed to allocate chunk of memory\n");
	} else {
		printf("500MB allocated\n");
	}
	printf("Allocating 50MB more...\n");
	b = (void *) malloc(50*1024*1024);
	if (b == NULL) {
		fprintf(stderr, "Failed to allocate chunk of memory\n");
	} else {
		printf("50MB allocated\n");
	}
	printf("De-allocating 500MB\n");
	free(a);
	printf("Sleeping for 10s\n");
	sleep(10);
}
