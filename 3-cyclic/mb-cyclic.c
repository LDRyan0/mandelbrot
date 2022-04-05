#define MAXITER 1000
#define N 800

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

/* ----------------------------------------------------------------*/

int main(int argc, char*argv[]) {
    int	   i, j, k, loop, procSize, rank, ncpu, pos;
    float  *x, *y, *out;
    float complex   z, kappa;
	FILE   *fp; /* for image .ppm or timing .csv file */

	/* Unused variables cause errors in compilation due to -Werror standard*/
	#ifndef TIME
	int green, blue;
	#endif

	#ifdef TIME
	double times[4];
	double *allTimes;
	double time1, time2;
	#endif

	/* Set up MPI */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ncpu);

	procSize = N * N / ncpu;

    x = (float *)malloc(procSize*sizeof(float));
	if (rank == 0) {
		y = (float *)malloc(N*N*sizeof(float));
		out = (float *)malloc(N*N*sizeof(float));
		#ifdef TIME
		allTimes = (double *)malloc(4*ncpu*sizeof(double));
		#endif
	}

	#ifdef TIME
	times[0] = (double)rank;
	time1 = MPI_Wtime();
	#endif
    for (loop=0; loop < procSize; loop++) {
	    pos = rank + loop * ncpu;
		i=pos/N;
	    j=pos%N;
	    z=kappa= (4.0*(i-N/2))/N + (4.0*(j-N/2))/N * I;
	
	    k=1;
	    while ((cabs(z)<=2) && (k++<MAXITER)) 
	        z= z*z + kappa;
	  
	    x[loop]= log((float)k) / log((float)MAXITER);
	}
	#ifdef TIME
	time2 = MPI_Wtime(); /* calcTime */
	times[1] = time2 - time1;
	MPI_Barrier(MPI_COMM_WORLD);
	time1 = MPI_Wtime();
	times[2] = time1 - time2; /* waitTime */
	#endif

	/* Naive gather, out of order */
	MPI_Gather(x, procSize, MPI_FLOAT, y, procSize, MPI_FLOAT, 0, MPI_COMM_WORLD);

	#ifdef TIME
	time2 = MPI_Wtime();
	times[3] = time2 - time1; /* commTime */
	MPI_Gather(&times, 4, MPI_DOUBLE, allTimes, 4, MPI_DOUBLE, 0, MPI_COMM_WORLD); /* get all timing info */
	#endif
	
	if (rank == 0) {
		#ifdef TIME
		time1 = MPI_Wtime();
		#endif
		/* Rearrange out-of-place into correct order */
		for (i = 0; i < procSize; i++) {
			for (j = 0; j < ncpu; j++) {
				out[i*ncpu + j] = y[i + j*procSize];  
			}
		}

		#ifdef TIME /* Write timing results to .csv file */
		time2 = MPI_Wtime();
		times[1] += time2 - time1; /* included added work by process 0 to rearrange */

		fp = fopen("cyclic-results.csv","w");
		fprintf(fp, "rank,calcTime,waitTime,commTime\n"); /* .csv header */
		for (i = 0; i < 4*ncpu; i+=4) {
			fprintf(fp, "%d,%lf,%lf,%lf\n", (int)allTimes[i], allTimes[i+1], allTimes[i+2], allTimes[i+3]);
		}
		fclose(fp);
		free(allTimes);
		#endif
		
		#ifndef TIME
	    printf("Writing mandelbrot3.ppm\n");
	    fp = fopen ("mandelbrot3.ppm", "w");
	    fprintf (fp, "P3\n%4d %4d\n255\n", N, N);
	    for (loop=0; loop<N*N; loop++) 
		    if (out[loop]<0.5) {
		        green= (int) (2*out[loop]*255);
	            fprintf (fp, "%3d\n%3d\n%3d\n", 255-green,green,0);
		    } else {
		        blue= (int) (2*out[loop]*255-255);
	            fprintf (fp, "%3d\n%3d\n%3d\n", 0,255-blue,blue);
		    }
	    
	    fclose(fp);
		free(y);
		#endif
		free(out);
	}


	free(x);

	MPI_Finalize();
	return 0;
}
