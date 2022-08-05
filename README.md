# Mandelbrot

A set of HPC C programs that produce the [Mandelbrot set](https://en.wikipedia.org/wiki/Mandelbrot_set) as a PPM image. Implementation of a variety of partionining philosophies through the use of message passing within [OpenMPI](https://www.open-mpi.org/).

0. Serial (naive)
1. Block partitioning (static)
2. Master-worker (dynamic) 
3. Cyclic partitioning (static)

# Usage
### Standard usage

```bash
git clone git@github.com:LDRyan0/mandelbrot.git
./make.sh
cd mandelbrot/2-master-worker/
srun -n 4 master-worker
xli mandelbrot2.ppm
```
### Timing usage
All programs utilise conditional compilation, add the `TIME` flag to the make script to enable timing functionality and disable image output.
```bash
./make.sh TIME
```
### Output validation
Generate all images and then run the checking script to validate output. Uses `diff` on the .ppm files.
```bash
./check.sh
```

Mandelbrot Set Image | Example Timing Analysis
|:-------------:|:------------:|
<img src="https://user-images.githubusercontent.com/86461236/161067232-41d770c1-7615-4e13-b937-e3ace8c7aef7.JPG" height="300"/> | <img src="https://user-images.githubusercontent.com/86461236/183006337-c5954c4a-91d2-4dd5-9c07-3c5cc1470e6b.png" height="300"/>
