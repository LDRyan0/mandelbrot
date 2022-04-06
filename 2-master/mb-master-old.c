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
	FILE   *fp;

	/* Unused variables cause errors in compilation due to -Werror standard*/
	#ifndef TIME
	int green, blue;
	#endif

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ncpu);

	#ifdef TIME
	double times[3];
	double time1, time2;
	double* allTimes;
	times[0] = (double)rank, times[1] = 0, times[2] = 0;
	#endif
		
	
	/* Working array */
	float *x;	
	x = (float *)malloc(chunkSize*sizeof(float));


	if (rank == MASTER) {
		#ifdef TIME
		allTimes = (double *)malloc(3*ncpu*sizeof(double));
		#endif

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

		#ifdef TIME
		MPI_Gather(&times, 3, MPI_DOUBLE, allTimes, 3, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		/* Sum up times across all processes and store in master area */
		for (i = 3; i < 3*ncpu; i+=3) {
			allTimes[1] += allTimes[i+1];
			allTimes[2] += allTimes[i+2];
		}

		/* write detailed (per process) timing */
		fp = fopen("results-master-all.csv","a");
		for (i = 3; i < 3*ncpu; i+=3) {
			fprintf(fp, "%d,%d,%lf,%lf\n", chunkSize, (int)allTimes[i], allTimes[i+1], allTimes[i+2]);
		}
		fclose(fp);

		/* write summarised (totals) timing */
		fp = fopen("results-master-short.csv","a");
		fprintf(fp, "%d,%lf,%lf\n", chunkSize, allTimes[1], allTimes[2]);
		fclose(fp);
		free(allTimes);
		#endif

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
			#ifdef TIME
			time1 = MPI_Wtime();
			#endif
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
			#ifdef TIME
			time2 = MPI_Wtime();
			times[1] += time2 - time1; /* calcTime */
			#endif
			
			/* Send completed work back to master, with startIdx in tag */
			MPI_Send(x, chunkSize, MPI_FLOAT, MASTER, startIdx, MPI_COMM_WORLD);	
			
			/* Receive startIdx for new work, will be -1 if finished*/
			MPI_Recv(&startIdx, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			#ifdef TIME
			time1 = MPI_Wtime();
			times[2] += time1 - time2; /* commTime */
			#endif
		}
		#ifdef TIME
		MPI_Gather(&times, 3, MPI_DOUBLE, allTimes, 3, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		#endif


	}
	free(x);	
}

int main(int argc, char*argv[]) {
	int chunks[] = {10, 30, 100, 300, 1000, 3000, 10000, 30000, 100000, 300000, 1000000, 3000000};
	int i;
	
	/* header */
	FILE *fp;

	fp = fopen("results-master-all.csv","w");
	fprintf(fp, "chunkSize,rank,calcTime,commTime\n");
	fclose(fp);

	fp = fopen("results-master-short.csv","w");
	fprintf(fp, "chunkSize,calcTime,commTime\n");
	fclose(fp);

	/* Set up MPI */
	MPI_Init(&argc, &argv);
	
	for (i = 0; i < sizeof(chunks) / sizeof(chunks[0]); i++) {
		mb_master_worker(chunks[i]);
		MPI_Barrier(MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}