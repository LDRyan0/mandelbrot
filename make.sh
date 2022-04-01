module load openmpi/2.1.2

CFLAGS="-lm -ansi -Werror"

# add any user specified flags 
for flag in "$@"
do
	CFLAGS="$CFLAGS -D $flag"
	echo "Added $flag to compilation flags"
done


FILES=$(find . -type f -regex ".*\.c")

# compile all programs
for f in $FILES
do	
	echo "mpicc $CFLAGS "${f%}" -o "${f%.*}""
	mpicc $CFLAGS "${f%}" -o "${f%.*}"
done

