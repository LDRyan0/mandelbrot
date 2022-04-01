/* Demonstration of the cyclic partitioning employed in 3-cyclic */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[]) {
	int i, j, n, rank, ncpu;
	int *x, *y, *z;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ncpu);

	n = 6;

	x = (int*)malloc(n*sizeof(int));
	if (rank == 0) {
		y = (int*)malloc(ncpu*n*sizeof(int));
		z = (int*)malloc(ncpu*n*sizeof(int));
	}

	for (i = 0; i < n; i++) {
		x[i] = rank + i * ncpu;
	}
	
	printf("(%d): x = [ ", rank);
	for (i = 0; i < n; i++) {
		printf("%2d ", x[i]);  	
	}
	printf("]\n");

	MPI_Gather(x, n, MPI_INT, y, n, MPI_INT, 0, MPI_COMM_WORLD);	
	MPI_Barrier(MPI_COMM_WORLD);
	fflush(stdout);

	if (rank == 0) {
		printf("\n(0): y = [ ");
		for (i = 0; i < ncpu*n; i++) {
			printf("%2d ", y[i]);  	
		}
		printf("]\n");
		
		/* Rearrange out-of-place into correct order */
		for (i = 0; i < n; i++) {
			for (j = 0; j < ncpu; j++) {
				z[i*ncpu + j] = y[i + j*n];  
			}
		}
		
		printf("(0): z = [ ");
		for (i = 0; i < ncpu*n; i++) {
			printf("%2d ", z[i]);  	
		}
		printf("]\n\n");
	}
	
	MPI_Finalize();
	return 0;
}
