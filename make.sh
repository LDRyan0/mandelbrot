module load openmpi/2.1.2

CFLAGS="-lm -ansi -Wall"
FILES=()

FILES=$(find . -type f -regex ".*\.c")

# compile all programs
for f in $FILES
do	
	echo "mpicc $CFLAGS "${f%}" -o "${f%.*}""
	mpicc $CFLAGS "${f%}" -o "${f%.*}"
done






