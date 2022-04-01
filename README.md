# Mandelbrot

A set of HPC C programs that produce the [Mandelbrot set](https://en.wikipedia.org/wiki/Mandelbrot_set) as a PPM image. Implementation of a variety of partionining philosophies through the use message passing within [OpenMPI](https://www.open-mpi.org/).

0. Serial (naive)
1. Block partitioning (static)
2. Master-worker (dynamic) 
3. Cyclic partitioning (static)

## Example usage

```bash
git clone git@github.com:LDRyan0/mandelbrot.git
cd mandelbrot/2-master-worker/
mpicc -lm mb-master-worker.c -o master-worker
srun -n 4 master-worker
xli mandelbrot2.ppm
```

<img src="https://user-images.githubusercontent.com/86461236/161067232-41d770c1-7615-4e13-b937-e3ace8c7aef7.JPG" height="300"/>
