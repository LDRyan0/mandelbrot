#!/bin/bash -l
#SBATCH --nodes=1
#SBATCH --tasks-per-node=10
#SBATCH --cpus-per-task=1
#SBATCH --reservation=GS-20880
#SBATCH --account=courses0100
#SBATCH --job-name=mandelbrot
#SBATCH --time=00:10:00
#SBATCH --export=NONE

module load openmpi/2.1.2
srun -N 1 -n 10 a.out
