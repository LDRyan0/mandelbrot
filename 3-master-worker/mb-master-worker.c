#define MAXITER 1000
#define N 200
#define MASTER 0

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <string.h>

/* ----------------------------------------------------------------*/

int main(int argc, char*argv[]) {
	int	   i, j, k, green, blue, loop, chunkSize, domainSize, rank, ncpu, startIdx, tag;
	FILE   *fp;
	float complex   z, kappa;
	MPI_Status status;
	MPI_Request request;
	/* Set up MPI */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ncpu);
			
	/* All processes aware of chunksize */
	chunkSize = N * N / 50; 		
	
	/* Working array */
	float *x;	
	x = (float *)malloc(chunkSize*sizeof(float));

	if (rank == MASTER) {
		/* Store extra elements to get perfect multiple of chunkSize */
		domainSize = ((N * N - 1) / chunkSize + 1) * chunkSize;
		printf("domainSize = %d\n", domainSize);		
		printf("ncpu       = %d\n", ncpu);		
		
		/* Master array */
		float *y;
		y = (float *)malloc(domainSize*sizeof(float));
		

		/* Workers already complete one "chunk" of work without communication
		   => begin startIdx at (ncpu-1)*chunkSize */
		startIdx = (ncpu - 1) * chunkSize;

		while (startIdx < domainSize) {
			/* Receive work from worker */
			MPI_Recv(x, chunkSize, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			tag = status.MPI_TAG;
			printf("MASTER (%d): Received [%8d,%8d] from (%d)\n", 0, tag, tag + chunkSize, status.MPI_SOURCE);
			printf("Copied successfully\n");
			//memcpy(y + tag, x, chunkSize*sizeof(float)); /* copy in work, index stored in tag */
			/* Send startIdx to worker asynchronously for new work */
			MPI_Send(&startIdx, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			printf("MASTER (%d): Sent     [%8d,%8d] to   (%d)\n\n", 0, startIdx, startIdx + chunkSize, status.MPI_SOURCE);
			startIdx += chunkSize;
		}

		startIdx = -1;
		loop = 1;
		printf("Wrapping up...\n");
		/* Once all work has been allocated collect final work and communicate all processes to stop */
		while (loop < ncpu) {
			MPI_Recv(x, chunkSize, MPI_FLOAT, MPI_ANY_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
			printf("Received data [%8d,%8d] from (%d)\n", status.MPI_TAG, status.MPI_TAG + chunkSize, status.MPI_SOURCE);
			tag = status.MPI_TAG;
			memcpy(y + tag, x, chunkSize*sizeof(float)); /* copy in work, index stored in tag */
			printf("status.MPI_SOURCE = %d", status.MPI_SOURCE);	
			/* Send startIdx to worker asynchronously for new work */
			MPI_Send(&startIdx, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			loop += 1;
		}

		/* ------------------------------ IO ------------------------------ */
 
		// printf("Writing mandelbrot2.ppm\n");
		// fp = fopen ("mandelbrot2.ppm", "w");
		// fprintf (fp, "P3\n%4d %4d\n255\n", N, N);
	    
		// for (loop=0; loop<N*N; loop++) 
		// 	if (y[loop]<0.5) {
		// 		green= (int) (2*y[loop]*255);
		// 		fprintf (fp, "%3d\n%3d\n%3d\n", 255-green,green,0);
		// 	} else {
		// 		blue= (int) (2*y[loop]*255-255);
		// 		fprintf (fp, "%3d\n%3d\n%3d\n", 0,255-blue,blue);
		// 	}
	    
		// fclose(fp);	

		/* ---------------------------------------------------------------- */

		free(y);
	}

	if (rank != MASTER) {
		int pos; /* For the actual position in the 2D master array */

		/* Working array */

		startIdx = (rank - 1) * chunkSize; /* For initial work */
 		
		while (startIdx >= 0) { /* Stop when processing finished */
			printf("WORKER (%d): Calculating [%8d,%8d]\n\n", 0, startIdx, startIdx + chunkSize, status.MPI_SOURCE);
			for (loop=0; loop < chunkSize; loop++) {
				pos = loop + startIdx;
				i=pos/N;
				j=pos%N;
				z=kappa= (4.0*(i-N/2))/N + (4.0*(j-N/2))/N * I;
				k=1;
				while ((cabs(z)<=2) && (k++<MAXITER)) 
					z= z*z + kappa;
			  
				x[loop]= log((float)k) / log((float)MAXITER);
			}
			printf("WORKER (%d): Finished [%8d,%8d]\n\n", 0, startIdx, startIdx + chunkSize, status.MPI_SOURCE);
			
			/* Send completed work back to master, with startIdx in tag */
			MPI_Send(x, chunkSize, MPI_FLOAT, MASTER, startIdx, MPI_COMM_WORLD);	
			printf("WORKER (%d): Sent     [%8d,%8d] to   (%d)\n", rank, startIdx, startIdx + chunkSize, 0);

			/* Receive startIdx for new work, -1 if finished*/
			MPI_Recv(&startIdx, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("WORKER (%d): Received [%8d,%8d] from (%d)\n\n", rank, startIdx, startIdx + chunkSize, 0);
		}

	}

	free(x);	
	MPI_Finalize();
	return 0;
}
