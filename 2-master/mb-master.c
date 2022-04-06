#define MAXITER 1000
#define N 8000
#define MASTER 0

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <string.h>

/* ----------------------------------------------------------------*/

void mb_master_worker(int chunkSize) {
	int	   i, j, k, loop, rank, ncpu, domainSize, startIdx, pos;
	float complex   z, kappa;
	MPI_Status status;
	
	#ifndef TIME
	FILE   *fp;
	#endif

	/* Unused variables cause errors in compilation due to -Werror standard*/
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ncpu);		
	
	/* Working array */
	float *x;	
	x = (float *)malloc(chunkSize*sizeof(float));

	if (rank == MASTER) {

		/* Store extra elements to get smallest sufficient multiple of chunkSize */
		domainSize = ((N * N - 1) / chunkSize + 1) * chunkSize;
		
		/* Master array */
		float *y;
		y = (float *)malloc(domainSize*sizeof(float));
		
		/* Workers already complete one "chunk" of work without communication */
		startIdx = (ncpu - 1) * chunkSize;

		while (startIdx < domainSize) {
			/* Receive work from worker */
			MPI_Recv(x, chunkSize, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			memcpy(y + status.MPI_TAG, x, chunkSize*sizeof(float)); /* copy in work, index stored in tag */
			
			/* Send startIdx back to worker we just received from for new work */
			MPI_Send(&startIdx, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			startIdx += chunkSize;
		}

		/* Once all work has been allocated collect final work and communicate all processes to stop */
		startIdx = -1;
		loop = 1;
		while (loop < ncpu) { /* while loop instead of for loop so final work can come asychronously */
			MPI_Recv(x, chunkSize, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			memcpy(y + status.MPI_TAG, x, chunkSize*sizeof(float)); /* copy in work, index stored in tag */
			/* Send -1 to worker so it knows to stop */
			MPI_Send(&startIdx, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			loop += 1;
		}

		#ifndef TIME
		printf("Writing mandelbrot2.ppm\n");
		fp = fopen ("mandelbrot2.ppm", "w");
		fprintf (fp, "P3\n%4d %4d\n255\n", N, N);
	    
		for (loop=0; loop<N*N; loop++) 
			if (y[loop]<0.5) {
				green= (int) (2*y[loop]*255);
				fprintf (fp, "%3d\n%3d\n%3d\n", 255-green,green,0);
			} else {
				blue= (int) (2*y[loop]*255-255);
				fprintf (fp, "%3d\n%3d\n%3d\n", 0,255-blue,blue);
			}
	    
		fclose(fp);	
		#endif

		free(y);
	}

	if (rank != MASTER) {
		startIdx = (rank - 1) * chunkSize; /* For initial work */
 		
		while (startIdx >= 0) { /* As we receive -1 when processing finished */
			for (loop=0; loop < chunkSize; loop++) {
				pos =  startIdx + loop;
				i=pos/N;
				j=pos%N;
				z=kappa= (4.0*(i-N/2))/N + (4.0*(j-N/2))/N * I;
				k=1;
				while ((cabs(z)<=2) && (k++<MAXITER)) 
					z= z*z + kappa;
			  
				x[loop]= log((float)k) / log((float)MAXITER);
			}
			
			/* Send completed work back to master, with startIdx in tag */
			MPI_Send(x, chunkSize, MPI_FLOAT, MASTER, startIdx, MPI_COMM_WORLD);	
			
			/* Receive startIdx for new work, will be -1 if finished*/
			MPI_Recv(&startIdx, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	free(x);	
}

int main(int argc, char*argv[]) {
	int chunks[] = {10, 30, 100, 300, 1000, 3000, 10000, 30000, 100000, 300000, 1000000, 3000000};
	int i, rank, ncpu;
	double time1, time2, execTime;
	
	/* header */
	FILE *fp;

	fp = fopen("results-master.csv","w");
	fprintf(fp, "chunkSize, execTime\n");

	/* Set up MPI */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ncpu);	

	for (i = 0; i < sizeof(chunks) / sizeof(chunks[0]); i++) {
		#ifdef TIME
		if (rank == 0) {
			time1 = MPI_Wtime();
		}
		#endif
		mb_master_worker(chunks[i]);
		MPI_Barrier(MPI_COMM_WORLD);
		#ifdef TIME
		if (rank == 0) {
			time2 = MPI_Wtime();
			execTime = time2 - time1;
			fprintf(fp,"%d,%lf\n", chunks[i], execTime);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		#endif
	}
	fclose(fp);

	MPI_Finalize();

	return 0;
}