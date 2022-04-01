#define MAXITER 1000
#define N 800

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

/* ----------------------------------------------------------------*/

int main(int argc, char*argv[]) {
    int	   i,j,k,green,blue,loop,procSize,rank,ncpu,pos;
    float  *x, *y, *out;
    FILE   *fp;
    float complex   z, kappa;

	
	/* Set up MPI */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ncpu);

	procSize = N * N / ncpu;

    x = (float *)malloc(procSize*sizeof(float));
	if (rank == 0) {
		y = (float *)malloc(N*N*sizeof(float));
		out = (float *)malloc(N*N*sizeof(float));
	}

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
	
	/* Naive gather, out of order */
	MPI_Gather(x, procSize, MPI_FLOAT, y, procSize, MPI_FLOAT, 0, MPI_COMM_WORLD);


	if(rank == 0) { 
		/* Rearrange out-of-place into correct order */
		for (i = 0; i < procSize; i++) {
			for (j = 0; j < ncpu; j++) {
				out[i*ncpu + j] = y[i + j*procSize];  
			}
		}
		
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
	}


    free(x);
	free(y);

	MPI_Finalize();
	return 0;
}
