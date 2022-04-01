# Compiles all .c programs in the directory using mpicc
# User can define preprocessor directives through command line arguments, eg:
#	./make.sh TIME

module load openmpi/2.1.2

CFLAGS="-lm -ansi -Wall -Werror"

# add any user specified flags 
for flag in "$@"
do
	CFLAGS="$CFLAGS -D $flag"
	echo "Added -D $flag to compilation flags"
done

FILES=$(find . -type f -regex ".*\.c")

# compile all programs
for f in $FILES
do	
	echo "mpicc $CFLAGS "${f}" -o "${f%.*}""
	mpicc $CFLAGS "${f}" -o "${f%.*}"
done

