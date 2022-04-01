#define MAXITER 1000
#define N	8000

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

/* ----------------------------------------------------------------*/

int main() {
    int	   i,j,k,green,blue,tenth;
    float  **x;
    FILE   *fp;

    float complex   z, kappa;

    /* Creating the 2D array */
    x=(float **) malloc(N*sizeof(float *));
    for (i=0;i<N;i++)
        x[i]=(float *) malloc(N*sizeof(float));
  
    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
	    z=kappa= (4.0*(i-N/2))/N + (4.0*(j-N/2))/N * I;
	
	    k=1;
	    while ((cabs(z)<=2) && (k++<MAXITER)) 
	        z= z*z + kappa;
	  
	    x[i][j]= log((float)k) / log((float)MAXITER);
        }

        tenth=(int) N/10;
        if ((i+1)%tenth==0) 
            printf("%d%% complete.\n",100*(i+1)/N);
    }

/* ----------------------------------------------------------------*/
  
    printf("Writing mandelbrot0.ppm\n");
    fp = fopen ("mandelbrot0.ppm", "w");
    fprintf (fp, "P3\n%4d %4d\n255\n", N, N);
    
    for (i=0; i<N; i++) 
        for (j=0; j<N; j++)
	    if (x[i][j]<0.5) {
	        green= (int) (2*x[i][j]*255);
                fprintf (fp, "%3d\n%3d\n%3d\n", 255-green,green,0);
	    } else {
	        blue= (int) (2*x[i][j]*255-255);
                fprintf (fp, "%3d\n%3d\n%3d\n", 0,255-blue,blue);
	    }
    
    fclose(fp);

/* ----------------------------------------------------------------*/

    for (i=0; i<N; i++)
        free(x[i]);
    free(x);
	return 0;
}
