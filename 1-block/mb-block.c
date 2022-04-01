#define MAXITER 1000
#define N 8000

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

/* ----------------------------------------------------------------*/

int main(int argc, char*argv[]) {
    int	   i, j, k, loop, groupSize, rank, ncpu, pos;
    float  *x, *y;
    float complex   z, kappa;

	#ifndef TIME
    FILE   *fp;
	int green, blue;
	#endif

	#ifdef TIME
	double time1, time2, calcTime, waitTime, commTime;
	#endif

	/* Set up MPI */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ncpu);
	
	groupSize = N * N / ncpu;

	/* Allocate arrays */ 
    x=(float *)malloc(groupSize*sizeof(float));	
	if (rank == 0) {
		y = (float *)malloc(N*N*sizeof(float));	
	}
	
	#ifdef TIME
	time1 = MPI_Wtime();
	#endif
    for (loop=0; loop<groupSize; loop++) {
	    pos = loop+groupSize*rank;
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
	calcTime = time2 - time1;
	MPI_Barrier(MPI_COMM_WORLD);
	time1 = MPI_Wtime();
	waitTime = time1 - time2;
	#endif

	MPI_Gather(x, groupSize, MPI_FLOAT, y, groupSize, MPI_FLOAT, 0, MPI_COMM_WORLD);
	
	#ifdef TIME
	time2 = MPI_Wtime();
	commTime = time2 - time1;
	printf("Process %d\n", rank);
	printf("\tCalc: %lf s\n", calcTime);
	printf("\tWait: %lf s\n", waitTime);
	printf("\tComm: %lf s\n", commTime);
	#endif



/* ----------------------------------------------------------------*/
	
	#ifndef TIME
	if(rank == 0) { 
	    printf("Writing mandelbrot1.ppm\n");
	    fp = fopen ("mandelbrot1.ppm", "w");
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
		free(y);
	}
	#endif

/* ----------------------------------------------------------------*/

    free(x);

	MPI_Finalize();
	return 0;
}
