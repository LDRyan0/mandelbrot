/* Demonstration of the MPI_Gather function */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[]) {
	int i, rank, ncpu, n;
	int *x, *y;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ncpu);

	n = 5;
	x = (int*)malloc(n*sizeof(int));

	for (i = 0; i < n; i++) {
		x[i] = i + rank*n;
	}
	
	printf("(%d): [ ", rank);
	for (i = 0; i < n; i++) {
		printf("%d ", x[i]);  	
	}
	printf("]\n");

	if (rank == 0) {
		y = (int*)malloc(ncpu*n*sizeof(int));
	}

	MPI_Gather(x, n, MPI_INT, y, n, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	fflush(stdout);

	if (rank == 0) {
		printf("\nMPI_Gather(x, 10, MPI_INT, y, 10, MPI_INT, 0, MPI_COMM_WORLD);\n\n");
		printf("(0): [ ");
		for (i = 0; i < ncpu*n; i++) {
			printf("%d ", y[i]);  	
		}
		printf("]\n");
		free(y);	
	}

	free(x);
	MPI_Finalize();
	return 0;	
}
