/* Demonstration of the MPI_Gather function */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[]) {
	int i, rank, ncpu;
	int *y;
	int x[10];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ncpu);


	for (i = 0; i < 10; i++) {
		x[i] = i + rank*10;
	}
	
	printf("(%d): [ ", rank);
	for (i = 0; i < 10; i++) {
		printf("%d ", x[i]);  	
	}
	printf("]\n");
	
	y = (int*)malloc(ncpu*10*sizeof(int));

	MPI_Gather(x, 10, MPI_INT, y, 10, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		printf("\nMPI_Gather(x, 10, MPI_INT, y, 10, MPI_INT, 0, MPI_COMM_WORLD);\n\n");
		printf("(0): [ ");
		for (i = 0; i < ncpu*10; i++) {
			printf("%d ", y[i]);  	
		}
		printf("]\n");
	}
	free(y);	

}
