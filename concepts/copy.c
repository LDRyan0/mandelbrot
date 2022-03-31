/* 	Demonstration of how the copy from worker into master array is
	done using memcpy() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char*argv[]) {
	int *x, *y;
	int i;
	/* ----------------------------------------- */
	x = (int *)malloc(4*sizeof(int));
	for (i = 0; i < 4; i++) {
		x[i] = i;
	}
	
	printf("x = [ ");
	for (i = 0; i < 4; i++) {
		printf("%d ", x[i]);
	}
	printf("]\n");
	
	/* ----------------------------------------- */
	
	y = (int *)malloc(10*sizeof(int));
	for (i = 0; i < 10; i++) {
		y[i] = i;
	}
	
	printf("y = [ ");
	for (i = 0; i < 10; i++) {
		printf("%d ", y[i]);
	}
	printf("]\n");
	
	/* ----------------------------------------- */
	
	memcpy(y+4, x, 4*sizeof(int));
	 
	printf("\nmemcpy(y+4, x, 4*sizeof(int))\n\n");
	printf("y = [ ");
	for (i = 0; i < 10; i++) {
		printf("%d ", y[i]);
	}
	printf("]\n");

}
