# runs all Mandelbrot programs with user specified number of process
module load openmpi/2.1.2


if [ $# -eq 1 ]
then
	FILES=$(find . -type f -regex ".\/[1-9].*\.c")
	# compile all programs
	for f in $FILES
	do	
		echo "srun -n $1 ${f%.*}"
		srun -n $1 ${f%.*}
	done
else
	RED='\033[0;31m'
	NC='\033[0m'
	echo -e "${RED}Incorrect usage${NC}: please specify number of processes, for example:"
	echo "./run.sh 10"
fi

